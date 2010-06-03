/*
	Тест mru-списка
*/

#include "mru.h"

#include <iostream>
#include <string>
#include <exception>
using namespace std;

#define TEST_TYPE string
#include "example.h"

template<class List>
void print_all(List &list)
{
	#if MRU_DEBUG
	list.test();
	#endif

	cout << "list: ";

	bool first = true;

	for(List::iterator iter = list.begin();
		iter != list.end(); iter++)
	{
		if (first)
			first = false;
		else
			cout << ", ";
		cout << iter->key() << "=" << iter->value();
	}

	if (first)
		cout << "(empty)";

	cout << endl << endl;
}

template<class List, class Key, class Value>
void insert(List &list, const Key &key, const Value &value)
{
	cout << "insert: " << key << "=" << value << endl;

	list[key] = value;
    
    /*-
	List::iterator iter = list.insert(key,value);
	
	#if MRU_DEBUG
	list.test();
	#endif
    -*/

	print_all(list);
}

template<class MruList>
struct sorter_by_key
{
	bool operator()(const typename MruList::item_type &i1,
		const typename MruList::item_type &i2)
	{
		return i1.key() < i2.key();
	}
};

template<class MruList>
struct remover_by_value
{
	typename MruList::value_type value;

	remover_by_value(typename MruList::value_type value)
		: value(value) {}

	bool operator()(const typename MruList::item_type &i)
	{
		return i.value() == value;
	}
};

int main()
{
	srand(0);

	try
	{

	cout << "\n********** mru::list<int, int>(1000) **********\n";

	typedef mru::list<int,int> int_mru;

	int_mru int_list(1000);
	int_mru::iterator it;

	cout << ".insert( 0 .. 10 )\n";
	for (int i = 0; i < 11; i++)
		int_list.insert(i,rand()%100);
	print_all(int_list);

	cout << ".sort( by value )\n";
	int_list.sort();
	print_all(int_list);

	cout << ".move( {1} to begin() )\n";
	int_list.move(int_list.begin(), 1);
	print_all(int_list);

	cout << ".find(1)->value() = 0" << endl;
	int_list.find(1)->value() = 0;
	print_all(int_list);

	cout << ".[7] = 0" << endl;
	int_list[7] = 0;
	print_all(int_list);

	cout << ".sort( by key )\n";
	int_list.sort(sorter_by_key<int_mru>());
	print_all(int_list);

	cout << ".erase( begin() )" << endl;
	int_list.erase( int_list.begin() );
	print_all(int_list);

	cout << ".remove(5)" << endl;
	int_list.remove(5);
	print_all(int_list);

	cout << ".remove_if( value == 0 )" << endl;
	int_list.remove_if( remover_by_value<int_mru>(0) );
	print_all(int_list);

	cout << ".clear()" << endl;
	int_list.clear();
	print_all(int_list);


	cout << "\n********** mru::list<tile_id, test>(4) **********\n";
	typedef mru::list<tile_id, test> tile_mru;
	
	tile_mru list(4);

	print_all(list);
	insert( list, tile_id(1,1,1,1), test("1.1") );
	insert( list, tile_id(2,2,2,2), test("2.1") );
	insert( list, tile_id(3,3,3,3), test("3.1") );
	insert( list, tile_id(4,4,4,4), test("4.1") );
	insert( list, tile_id(5,5,5,5), test("5.1") );
	insert( list, tile_id(2,2,2,2), test("2.2") );
	insert( list, tile_id(4,4,4,4), test("4.2") );
	insert( list, tile_id(1,1,1,1), test("1.2") );
	insert( list, tile_id(1,1,1,1), test("1.3") );

	cout<< "!!!\n";
	cout.flush();

	cout << "find(" << tile_id(4,4,4,4) << ")";
	tile_mru::iterator iter = list.find( tile_id(4,4,4,4) );
	if (iter == list.end())
		cout << " - not found\n";
	else
		cout << " - found: " << iter->key() << "=" << iter->value().a << endl;
	print_all(list);

	cout << "find(" << tile_id(3,3,3,3) << ")";
	iter = list.find( tile_id(3,3,3,3) );
	if (iter == list.end())
		cout << " - not found\n";
	else
		cout << " - found: " << iter->key() << "=" << iter->value().a << endl;
	print_all(list);

	cout << "[" << tile_id(6,6,6,6) << "]=" << test("6.1") << endl;
	list[ tile_id(6,6,6,6) ] = test("6.1");
	print_all(list);

	cout << "remove " << tile_id(4,4,4,4) << endl;
	list.remove( tile_id(4,4,4,4) );
	print_all(list);

	cout << "=[" << tile_id(4,4,4,4) << "]" << endl;
	list[ tile_id(4,4,4,4) ];
	print_all(list);

	cout << "up(" << tile_id(2,2,2,2) << ")" << endl;
	list.up( tile_id(2,2,2,2) );
	print_all(list);

	cout << "=[" << tile_id(1,1,1,1) << "]" << endl;
	list[ tile_id(1,1,1,1) ];
	print_all(list);

	cout << ".begin()=" << *list.begin() << endl;
	cout << ".front()=" << list.front() << endl;
	cout << ".back()=" << list.back() << endl;
	print_all(list);
	
	cout << ".erase(.begin())" << endl;
	list.erase(list.begin());
	print_all(list);

	cout << ".erase(++ .begin())" << endl;
	list.erase(++list.begin());
	print_all(list);

	insert( list, tile_id(3,3,3,3), test("3.2") );
	insert( list, tile_id(5,5,5,5), test("5.2") );

	cout << ".move( ++begin() to end() )" << endl;
	list.move( list.end(), (++list.begin())->key());
	print_all(list);

	cout << ".sort( by key )\n";
	list.sort(sorter_by_key<tile_mru>());
	print_all(list);

	cout << "clear()" << endl;
	list.clear();
	print_all(list);

	cout << "end\n";

	}
	catch(std::exception &e)
	{
		cout << "EXCEPTION: " << e.what() << endl;
	}

	return 0;
}
