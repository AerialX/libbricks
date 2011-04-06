#pragma once

#ifndef BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/collections.h"

#include <map>

namespace Bricks { namespace Collections {
	template<typename TKey, typename TValue> class DictionaryIterator;

	template<typename TKey, typename TValue>
	class Pair : public Object
	{
	private:
		TKey key;
		TValue value;
		TValue* pointer;

	public:
		Pair() { }
		Pair(const TKey& key, const TValue& value) : key(key), value(value), pointer(&const_cast<TValue&>(value)) { }
		Pair(typename std::map< TKey, TValue >::const_iterator iter) : key(iter->first), value(iter->second), pointer(NULL) { }
		Pair(typename std::map< TKey, TValue >::iterator iter) : key(iter->first), value(iter->second), pointer(&iter->second) { }

		TKey& GetKey() { return key; }
		const TKey& GetKey() const { return key; }
		TValue& GetValue() { return value; }
		const TValue& GetValue() const { return value; }
		void SetValue(const TValue& value) { this->value = value; if (pointer) *pointer = value; }
	};

	template<typename TKey, typename TValue>
	class Dictionary : public Object, public Collection< Pair<TKey, TValue> >
	{
	private:
		typename std::map< TKey, TValue > map;
		typedef Pair< TKey, TValue > mapitem;
		typedef Dictionary< TKey, TValue > dicttype;
		typedef DictionaryIterator< TKey, TValue > dictiter;
		typedef typename std::map< TKey, TValue >::iterator iterator;
		typedef typename std::map< TKey, TValue >::const_iterator const_iterator;

		friend class DictionaryIterator< TKey, TValue >;

		iterator IteratorOfValue(const TValue& value) {
			for (iterator iter = map.begin(); iter != map.end(); iter++) {
				if (iter->second == value)
					return iter;
			}
			return map.end();
		}

		const_iterator IteratorOfValue(const TValue& value) const {
			for (const_iterator iter = map.begin(); iter != map.end(); iter++) {
				if (iter->second == value)
					return iter;
			}
			return map.end();
		}

	public:
		Dictionary() { }
		Dictionary(const Dictionary< TKey, TValue >& dictionary) : map(dictionary.map) { }
		Dictionary(const Collection< Pair< TKey, TValue > >& collection) { AddItems(collection); }
		virtual ~Dictionary() { }

		TValue& GetItem(const TKey& key) { return map[key]; }
		const TValue& GetItem(const TKey& key) const { iterator iter = map.find(key); if (iter == map.end()) throw InvalidArgumentException(); return iter->second; }
		const Collection<const TValue&>& GetValues() const;
		const Collection<const TKey&>& GetKeys() const;

		bool ContainsKey(const TKey& key) const { return map.find(key) != map.end(); }
		bool ContainsValue(const TValue& value) const { return IteratorOfValue(value) != map.end(); }

		void Add(const TKey& key, const TValue& value) { map[key] = value; }
		bool RemoveKey(const TKey& key) { return map.erase(key); }
		bool RemoveValue(const TValue& value) { iterator iter = IteratorOfValue(value); if (iter == map.end()) return false; map.erase(iter); return true; }
		
		// Collections
		long GetCount() const { return map.size(); }

		bool ContainsItem(const Pair< TKey, TValue >& value) const { const_iterator iter = map.find(value.GetKey()); return iter != map.end() && iter->second == value.GetValue(); }

		void AddItem(const Pair< TKey, TValue >& value) { map[value.GetKey()] = value.GetValue(); }
		void AddItems(Iterable< Pair< TKey, TValue> >& values) { foreach (const mapitem& value, values) AddItem(value); }
		void AddItems(Collection< Pair< TKey, TValue> >& values) { AddItems(static_cast<Iterable< Pair< TKey, TValue > >&>(values)); }
		bool RemoveItem(const Pair< TKey, TValue >& value) { return map.erase(value.GetKey()); }
		void Clear() { map.clear(); }
		
		// Iterator
		Iterator< Pair< TKey, TValue > >& GetIterator() const { return autoalloc dictiter(const_cast<dicttype&>(self)); }
		
		TValue& operator[](const TKey& key) { return GetItem(key); }
		const TValue& operator[](const TKey& key) const { return GetItem(key); }
	};
	
	template<typename TKey, typename TValue>
	class DictionaryIterator : public Object, public Iterator< Pair< TKey, TValue > >
	{
	private:
		bool first;
		typename std::map< TKey, TValue >::iterator position;
		typename std::map< TKey, TValue >::iterator end;
		mutable Pair< TKey, TValue > current;
		DictionaryIterator(Dictionary< TKey, TValue >& dictionary) : first(false), position(dictionary.map.begin()), end(dictionary.map.end()) { }

		friend class Dictionary< TKey, TValue >;

	public:
		virtual Pair< TKey, TValue >& GetCurrent() const { if (!first || position == end) throw InvalidIteratorException(); return const_cast<Pair < TKey, TValue >&>(current = Pair< TKey, TValue >(position)); }
		virtual bool MoveNext() { if (!first) return (first = true) && position != end; return position != end && ++position != end; }
	};
} }
