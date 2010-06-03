/*
	Used by speedtest.cpp only

	mru_list based on example for Boost.MultiIndex

	serialization.cpp

	Copyright 2003-2008 Joaquin M Lopez Munoz.
	Distributed under the Boost Software License, Version 1.0.
	(See accompanying file LICENSE_1_0.txt or copy at
	http://www.boost.org/LICENSE_1_0.txt)
	See http://www.boost.org/libs/multi_index for library home page.
*/

#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

#include <boost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
//#include <algorithm>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/functional/hash.hpp>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

template <typename Item>
class mru_list
{
	typedef boost::multi_index::multi_index_container
	<
		Item,
		boost::multi_index::indexed_by
		<
			boost::multi_index::sequenced<>,
            boost::multi_index::hashed_unique
			<
				boost::multi_index::identity<Item>
			>
		>
	>
	item_list;

private:
	item_list item_list_;
	size_t max_num_items_;

public:
	typedef Item item_type;
	typedef typename item_list::iterator iterator;

	mru_list(std::size_t max_num_items)
		: max_num_items_(max_num_items) {}

	void insert(const item_type& item)
	{
		std::pair<iterator,bool> p = item_list_.push_front(item);

		/* Если уже есть такой, поднимаем наверх */
		//if (!p.second)
		//	item_list_.relocate(item_list_.begin(), p.first);

		/* Удаляем самый старый */
		//else if (item_list_.size() > max_num_items_)
		//	item_list_.pop_back();
	}

	void clear()
	{
		item_list_.clear();
	}

	iterator begin() { return item_list_.begin(); }
	iterator end() { return item_list_.end(); }
};
