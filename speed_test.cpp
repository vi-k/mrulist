/*
	Тест скорости
	1) boost::unordered_map
	2) std::map
	3) std::list - для сравнения с mru (включить - TEST_STD_LIST)
	4) mru_list из примера boost::multi_index -> serialization.cpp
		(по умолчанию отключен, т.к. ОЧЕНЬ медленный - чтобы
		включить, установите TEST_MRU_LIST)
	5) собственный mru::list (boost::unordered_map + std::list)
	6) MruCache (http://www.codeproject.com/KB/stl/cpp_mru_cache.aspx)
		- модифицированный для проверки (std::map + std::list).
		В Debug-режиме не работает (включить - TEST_MRU_CACHE)
*/

//#define TEST_MRU_LIST
#define TEST_STD_LIST
//#define TEST_MRU_CACHE

#include "mru_list.h"
#include "MruCache.h"
#include "mru.h"

#include "example.h"

#include <stdlib.h>

#include <map>
#include <algorithm> /* generate, find */
#include <vector>
#include <string>
#include <iostream>
#include <memory> /* auto_ptr */
using namespace std;

#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <boost/shared_ptr.hpp>

/* Boost'овское время (и чтоб без подключения библиотек) */
#include <libs/date_time/src/posix_time/posix_time_types.cpp>
#include <libs/date_time/src/gregorian/date_generators.cpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#define SIZE 1000

#define KEY_TYPE   1  /* 0 - int, 1 - tile_id          */
#define VALUE_TYPE 1  /* 0 - int, 1 - shared_ptr<test> */



#if KEY_TYPE==0
typedef int Key;
#elif KEY_TYPE==1
typedef tile_id Key;
#endif

#if VALUE_TYPE==0
typedef int Value;
#elif VALUE_TYPE==1
typedef boost::shared_ptr<test> Value;
#endif


struct random_key
{
	Key operator()() const
	{
		#if KEY_TYPE==0
		return rand();
		#elif KEY_TYPE==1
		int map_id = rand() % 4 + 1;
		int z = rand() % 20 + 1;
		int c = 1 << (z - 1);
		int x = rand() % c;
		int y = rand() % c;
		return tile_id(map_id, z, x, y);
		#endif
	}
};

struct random_value
{
	Value operator()() const
	{
		#if VALUE_TYPE==0
		return rand();
		#elif VALUE_TYPE==1
		return Value( new test(rand()) );
		#endif
	}
};

vector<Key> keys(SIZE);
vector<Value> values(SIZE);
Key g_key;

void after_test()
{
	#if VALUE_TYPE==0
	
    //
	
	#elif VALUE_TYPE==1
	int shared = 0;
	for (vector<Value>::iterator iter = values.begin();
		iter != values.end(); iter++)
	{
		shared += iter->use_count();
	}
	cout <<"  shared=" << shared
		<< "  count=" << test::count();
	#endif

	cout << endl;
	cout.flush();
}

int main(int argc, char *argv[])
{
	try
	{

	using namespace boost::posix_time;

	int n = 1000;
	if (argc > 1)
		n = atoi(argv[1]);
	cout << "n=" << n << endl;
	cout << "Iterations = n*" << SIZE << " = " << n * SIZE << endl;
	cout << endl;

	srand(0);
	generate(keys.begin(), keys.end(), random_key());
	generate(values.begin(), values.end(), random_value());

	cout << "before start:";
	after_test();

	cout << "\n*** fill ***\n";

   	boost::unordered_map<Key,Value> tiles1;
	{
		cout << "boost::unordered_map: ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			tiles1.clear();

			ptime start = microsec_clock::local_time();
			for (int i = 0; i < SIZE; i++)
			{
				tiles1[ keys[i] ] = values[i];
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();

		int failed = 0;
		for (int i = 0; i < SIZE; i++)
		{
			if (tiles1[ keys[i] ] != values[i])
				failed++;
		}
		
		cout << "  failed=" << failed;
		cout.flush();
	}

	after_test();

   	map<Key,Value> tiles2;
	{
		cout << "std::map:             ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			tiles2.clear();

			ptime start = microsec_clock::local_time();
			for (int i = 0; i < SIZE; i++)
			{
				tiles2[ keys[i] ] = values[i];
			}
			time += microsec_clock::local_time() - start;
		}

		cout << to_simple_string(time);
		cout.flush();

		int failed = 0;
		for (int i = 0; i < SIZE; i++)
		{
			if (tiles2[ keys[i] ] != values[i])
				failed++;
		}

		cout << "  failed=" << failed;
		cout.flush();
	}

	after_test();

#ifdef TEST_MRU_LIST

   	mru_list<tile_id> tiles3(SIZE);
	{
		cout << "mru_list:             ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			tiles3.clear();

			ptime start = microsec_clock::local_time();
			for (int i = 0; i < SIZE; i++)
			{
				tiles3.insert(keys[i]);
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

#endif

#ifdef TEST_STD_LIST

   	list<Key> stdlist1;
	{
		cout << "std::list(key):       ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			stdlist1.clear();

			ptime start = microsec_clock::local_time();
			for (int i = 0; i < SIZE; i++)
			{
				stdlist1.push_back(keys[i]);
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

   	list<Value> stdlist2;
	{
		cout << "std::list(value):     ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			stdlist2.clear();

			ptime start = microsec_clock::local_time();
			for (int i = 0; i < SIZE; i++)
			{
				stdlist2.push_back( values[i] );
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

#endif

	typedef mru::list<Key,Value> tile_mru;
   	tile_mru tiles4(SIZE);
	{
		cout << "mru::list:            ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			tiles4.clear();

			ptime start = microsec_clock::local_time();
			for (int i = 0; i < SIZE; i++)
			{
				tiles4[ keys[i] ] = values[i];
				//tiles4.test();
			}
			time += microsec_clock::local_time() - start;
		}

		cout << to_simple_string(time);
		cout.flush();

		int failed = 0;
		for (int i = 0; i < SIZE; i++)
		{
			if (tiles4[ keys[i] ] != values[i])
				failed++;
		}
		
		cout << "  failed=" << failed;
		cout.flush();
	}

	after_test();

#ifdef TEST_MRU_CACHE

	MruCache<Key,Value> tiles5(SIZE);
	{
		cout << "MruCache:             ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			tiles5.Clear();

			ptime start = microsec_clock::local_time();
			for (int i = 0; i < SIZE; i++)
			{
				tiles5.InsertItem(keys[i], values[i]);
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();

		int failed = 0;
		for (int i = 0; i < SIZE; i++)
		{
			if (tiles5.FindItem(keys[i]) != values[i])
				failed++;
		}
		
		cout << "  failed=" << failed;
		cout.flush();
	}

	after_test();

#endif

	cout << "\n*** access ***\n";

	{
		cout << "boost::unordered_map: ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			ptime start = microsec_clock::local_time();
			for (int i = SIZE - 1; i >= 0; i--)
			{
				values[i] = tiles1.find(keys[i])->second;
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

	{
		cout << "std::map:             ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			ptime start = microsec_clock::local_time();
			for (int i = SIZE - 1; i >= 0; i--)
			{
				values[i] = tiles2.find(keys[i])->second;
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

#ifdef TEST_STD_LIST

	{
		cout << "std::list(key):       ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			ptime start = microsec_clock::local_time();
			for (int i = SIZE - 1; i >= 0; i--)
			{
				g_key = *find(stdlist1.begin(), stdlist1.end(), keys[i]);
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

#endif

	{
		cout << "mru::list:            ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			ptime start = microsec_clock::local_time();
			for (int i = SIZE - 1; i >= 0; i--)
			{
				values[i] = tiles4.find(keys[i])->value();
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

	{
		cout << "mru::list[]:          ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			ptime start = microsec_clock::local_time();
			for (int i = SIZE - 1; i >= 0; i--)
			{
				values[i] = tiles4[keys[i]];
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

#ifdef TEST_MRU_CACHE
	{
		cout << "MruCache:             ";
		cout.flush();

		time_duration time;
		for (int j = 0; j < n; j++)
		{
			ptime start = microsec_clock::local_time();
			for (int i = SIZE - 1; i >= 0; i--)
			{
				values[i] = tiles5.FindItem(keys[i]);
			}
			time += microsec_clock::local_time() - start;
		}
		
		cout << to_simple_string(time);
		cout.flush();
	}

	after_test();

#endif

	cout << endl;

	/* Удаление всех */
	tiles1.clear();
	tiles2.clear();
	#ifdef TEST_MRU_LIST
	tiles2.clear();
	#endif
	tiles4.clear();
	#ifdef TEST_MRU_CACHE
	tiles5.Clear();
	#endif
	#ifdef TEST_STD_LIST
	stdlist1.clear();
	stdlist2.clear();
	#endif

	cout << "before last clear:";
	after_test();
	
    /* Проверка для Value=shared_ptr<test>,
	   	что все объекты учтены правильно.
	   	Оповещение "It is a last test::~test()"
	   	должно быть до "end" */
	values.clear();

	cout << "after last clear:";
	after_test();

	cout << "end" << endl;

	}
	catch(std::exception &e)
	{
		cout << "EXCEPTION: " << e.what() << endl;
	}

	return 0;
}
