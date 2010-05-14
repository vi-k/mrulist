/*
	Тест mru-списка
*/

#include "mru.h"

#include <iostream>
#include <string>
using namespace std;

#define TEST_TYPE string
#include "example.h"

template<class List>
void print_all(const List &list)
{
	cout << "list: ";

	bool first = true;

	for(List::const_iterator iter = list.begin();
		iter != list.end(); iter++)
	{
		if (first)
			first = false;
		else
			cout << ", ";
		cout << iter->key() << "=" << iter->value();
	}

	cout << endl << endl;
}

template<class List, class Key, class Value>
void insert(List &list, const Key &key, const Value &value)
{
	List::iterator iter = list.insert(key,value);
	cout << "insert: " << iter->key() << "=" << iter->value() << endl;

	print_all(list);
}

int main()
{
	typedef mru::list<int,int> int_mru;
	int_mru int_list(1000);
	for (int i = 0; i < 11; i++)
		int_list.insert(i,i);
	int_list.clear();

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

	cout << "find(" << tile_id(1,1,1,1) << ")";
	tile_mru::iterator iter = list.find( tile_id(1,1,1,1) );
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

	cout << "clear()" << endl;
	list.clear();
	print_all(list);

	cout << "end\n";

	return 0;
}
