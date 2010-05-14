#ifndef MRU_H
#define MRU_H

/*
	MRU-list собственного производства.

	Создан на основе:
		boost::unordered_map (далее map) - для быстрого доступа к значению
		по уникальному ключу (key). Соответственно, для класса Key
		должна быть определена функция hash_value (см. boost::hash).
		
		std::list (далее list) - для хранения значений в порядке последнего
		использования.

	Суть:
		Map строится по ключу (key) и хранит итераторы list'а (надеюсь,
		на свой страх и риск, что они не меняются в результате
		добавления/удаления элементов) и больше ничего.

		Элементы list'а хранят указатели на mru::list (об этом далее),
		сами значения (value) и... Вот здесь два варианта:
		1) ключи (если MRU_USE_MAP_ITER == 0). Соответственно, и доступ
			к элементам map'а (пока только для их удаления) будет вестить
			по ключу.
		2) итераторы (MRU_USE_MAP_ITER == 1). Если сделать предположение
			о неизменяемости итераторов map'а, то, теоретически, доступ
			по итератору должен быть быстрее, чем по ключу. Но! столкнулся
			с непонятной ошибкой - удаление по итератору через некоторое
			время приводит к сбою. Пока, стабильно это происходит при
			добавлении в список хотя бы 11 элементов и последующем их
			удалении через clear(). Поэтому, пока, по умолчанию
			MRU_USE_MAP_ITER == 0.

		Mru::list, соответственно, хранит и map, и list, и предоставляет
		интерфейс для работы с последним. Все функции, возвращающие
		итератор, возвращают итератор именно list'а. Map для пользователя
		не доступен. Mru::list, также, хранит ограничитель на кол-во
		элементов в списке (max_items), но сам его никак не использует.
		
		За добавление/удаление/ограничение элементов в списке отвечает (!)
		элемент list'а - его конструктор и деструктор. Это, с одной стороны,
		вынуждает хранить в каждом из них указатель на mru::list. Но,
		с другой стороны, пользователь, получив итератор list'а, сможет
		сам удалять и перемещать элементы без каких-либо последствий.
*/

//#include <iostream> /* Для отладки */
#include <list>
#include <boost/unordered_map.hpp>

#define MRU_USE_MAP_ITER 0

namespace mru
{

template <typename Key, typename Value>
class list
{
private:
    class item;
	typedef std::list<item> list_t;
	typedef boost::unordered_map<Key, typename list_t::iterator> map_t;
	typedef typename map_t::iterator map_iterator;

public:
	typedef typename list_t::iterator iterator;
	typedef typename list_t::const_iterator const_iterator;

private:

	class item
	{
	private:
		list &mru_;
		#if MRU_USE_MAP_ITER
		map_iterator iter_;
		#else
		Key key_;
		#endif
		Value value_;
		bool mapped_;

		#if MRU_USE_MAP_ITER
		item(list &mru, const Value &value)
			: mru_(mru)
			, value_(value)
			, mapped_(false) {}
		#else
		item(list &mru, const Key &key, const Value &value)
			: mru_(mru)
			, key_(key)
			, value_(value)
			, mapped_(false) {}
		#endif

	public:
		~item()
		{
			if (!mapped_) return;

			/* Удаляем из map */
			#if MRU_USE_MAP_ITER
			/* Вот, здесь, на любой из этих функций
				тот самый сбой и происходит */
			//mru_.map_.erase(iter_);
			mru_.map_.erase_return_void(iter_); /*boost <= :1.42*/
			//mru_.map_.quick_erase(iter_); /*boost >= 1.43*/

			/* При этом доступ к самому map'у проходит вполне успешно,
				и удаление по ключу map'а совершается без сбоев */
			//mru_.map_.erase(iter_->first);

			#else
			mru_.map_.erase(key_);
			#endif
		}

		static iterator create(list &mru, const Key &key, const Value &value)
		{
		    /* Ищем ключ */
		    map_iterator map_iter = mru.map_.find(key);

			/* Если уже есть - удаляем */
			if (map_iter != mru.map_.end())
				mru.list_.erase(map_iter->second);

			//std::cout << "before insert: list.size=" << mru.list_.size()
			//	<< " map.size=" << mru.map_.size()
			//	<< std::endl;

	        /* Добавляем в list */
			iterator list_iter = mru.list_.insert(
				mru.list_.begin(),
				#if MRU_USE_MAP_ITER
				item(mru, value)
				#else
				item(mru, key, value)
				#endif
				);

	        /* Добавляем в map */
			std::pair<map_t::iterator, bool> p
				= mru.map_.insert(map_t::value_type(key, list_iter));

			#if MRU_USE_MAP_ITER
	        /* Меняем в list ссылку на map */
    	    list_iter->iter_ = p.first;
    	    #endif

    	    list_iter->mapped_ = true;

			/* Удаляем лишние */
			if (mru.list_.size() > mru.max_items_)
				mru.list_.pop_back();

			//std::cout << "after insert: list.size=" << mru.list_.size()
			//	<< " map.size=" << mru.map_.size()
			//	<< std::endl;

    	    return list_iter;
		}

		inline const Key& key() const
		{
			#if MRU_USE_MAP_ITER
			return iter_->first;
			#else
			return key_;
			#endif
		}

		inline Value& value()
			{ return value_; }

		inline const Value& value() const
			{ return value_; }
	};

	map_t map_;
	list_t list_;
	size_t max_items_;

public:
	list(size_t max_items)
		: max_items_(max_items) {}

    /* Добавление нового элемента - всегда наверх */
	inline iterator insert(Key const& key, Value const& value)
	{
		return item::create(*this, key, value);
	}

    /* Поиск по ключу */
	inline iterator find(Key const& key)
	{
		map_iterator map_iter = map_.find(key);
		return (map_iter == map_.end() ? list_.end() : map_iter->second);
	}

    /* Удаление по ключу */
	void remove(Key const& key)
	{
		map_iterator map_iter = map_.find(key);
		if (map_iter != map_.end())
			list_.erase(map_iter->second);
	}

	iterator up(Key const& key)
	{
		map_iterator map_iter = map_.find(key);
		if (map_iter == map_.end())
			return list_.end();

		iterator list_iter = map_iter->second;

		/* Поднимаем наверх */
		list_t tmp_list;
		tmp_list.splice(tmp_list.begin(), list_, list_iter);
		list_.splice(list_.begin(), tmp_list);

        /* Надо что-то сделать с итератором в map'е */
        map_iter->second = list_iter;

		return list_iter;
	}

    /*
    	Доступ по ключу.
    	
    	Если ключ отсутствует, он будет создан - у класса Value
    	должен быть определён конструктор по умолчанию.

		Если ключ уже есть, он будет поднят наверх.
    */
	Value& operator[](Key const& key)
	{
		map_iterator map_iter = map_.find(key);
		iterator list_iter;

		/* Т.к. обязательно нужно вернуть результат,
			то, если значение ещё не существует, добавляем его.
			Для этого для Value должен быть определён конструктор
			по умолчанию! */
		if (map_iter == map_.end())
			list_iter = item::create(*this, key, Value());
		else
		{	
			list_iter = map_iter->second;

			/* Поднимаем наверх - медленная операция */
			list_t tmp_list;
			tmp_list.splice(tmp_list.begin(), list_, list_iter);
			list_.splice(list_.begin(), tmp_list);
			list_iter = list_.begin();

	        /* Надо что-то сделать с итератором в map'е */
			map_iter->second = list_iter;
		}

		return list_iter->value();
	}

	inline void clear()
		{ list_.clear(); }

	inline iterator begin()
		{ return list_.begin(); }

	inline const_iterator begin() const
		{ return list_.begin(); }
	
	inline iterator end()
		{ return list_.end(); }
	
	inline const_iterator end() const
		{ return list_.end(); }
};

}

#endif
