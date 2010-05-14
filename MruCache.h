/*
	Used by speedtest.cpp only
*/

#include <list>
#include <map>

/**
 * MRU Cache (http://www.codeproject.com/KB/stl/cpp_mru_cache.aspx)
 *
 * contains up to a fixed size of "Most Recently Used" items,
 * where items are assiciated with keys.
 * when asked to fetch a value that is not
 * in the cache, HandleNonExistingKeyFetch is called.
 * when an item is removed from the cache, HandleItemRelease is called.
 * implementor of this cache must provide those methods.
 *
 */
template <typename key_type, typename value_type>
class MruCache
{
public:
    
    const int maxLength;

    MruCache(int iMaxLength) : maxLength(iMaxLength) { }

    inline void InsertItem(key_type key, value_type value)
    	{ __insert_item(key, value); }

    inline value_type FindItem(key_type key)
    	{ return __find_item(key); }

    virtual ~MruCache() { Clear(); }

    /**
     * clear the cache.
     * for every item contained, HandleItemRelease is called.
     */
    virtual void Clear() { __clear(); }


protected:

    //virtual void HandleItemRelease(key_type key, value_type value) { };

    //virtual value_type HandleNonExistingKeyFetch(key_type key) = 0;

private:

    typedef struct _Entry
    {
        key_type key;
        value_type value;
    } Entry;

    typedef std::list<Entry> EntryList;
    EntryList listOfEntries;

    /**
     * for fast search in the cache.
     * this map contains pointers to iterators in EntryList.
     */
    typedef std::map<key_type, void*> ItrPtrMap;
    ItrPtrMap mapOfListIteratorPtr;

    void __insert_item(key_type key, value_type value)
    {
        Entry entry;
        EntryList::iterator* ptrItr = 
           (EntryList::iterator*) mapOfListIteratorPtr[key];
        if (!ptrItr)
        {
            if ( (int)listOfEntries.size() >= maxLength)
            {
                Entry lruEntry = listOfEntries.back();
                //HandleItemRelease(lruEntry.key, lruEntry.value);
                listOfEntries.pop_back();
                delete mapOfListIteratorPtr[lruEntry.key];
                mapOfListIteratorPtr.erase(lruEntry.key);
            }

            entry.value = value; //HandleNonExistingKeyFetch(key);
            entry.key = key;
            listOfEntries.push_front(entry);

            EntryList::iterator* ptrItr = new EntryList::iterator();
            *ptrItr = listOfEntries.begin();
            mapOfListIteratorPtr[key] = ptrItr;
        }
        else
        {
            entry = *(*ptrItr);
            entry.value = value;
            listOfEntries.erase(*ptrItr);
            listOfEntries.push_front(entry);
            *ptrItr = listOfEntries.begin();
        }
    }

    value_type __find_item(key_type key)
    {
        Entry entry;
        EntryList::iterator* ptrItr = 
           (EntryList::iterator*) mapOfListIteratorPtr[key];
        if (!ptrItr)
        	throw "Item not found";

        entry = *(*ptrItr);
        listOfEntries.erase(*ptrItr);
        listOfEntries.push_front(entry);
        *ptrItr = listOfEntries.begin();

        return entry.value;
    }

    virtual void __clear()
    {
        for (ItrPtrMap::iterator i=mapOfListIteratorPtr.begin(); 
             i!=mapOfListIteratorPtr.end(); i++)
        {
            void* ptrItr = i->second;
            EntryList::iterator* pItr = (EntryList::iterator*) ptrItr;
            //HandleItemRelease( (*pItr)->key, (*pItr)->value );
            delete ptrItr;
        }
        listOfEntries.clear();
        mapOfListIteratorPtr.clear();
    }
};
