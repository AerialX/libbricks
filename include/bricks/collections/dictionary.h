#pragma once

#include "bricks/config.h"

#if !BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/collections/comparison.h"
#include "bricks/collections/collection.h"

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
		Pair(const TKey& key, const TValue& value) : key(key), value(value), pointer(NULL) { }
		Pair(typename std::map<TKey, TValue>::const_iterator iter) : key(iter->first), pointer(const_cast<TValue*>(iter->second)) { }
		Pair(typename std::map<TKey, TValue>::iterator iter) : key(iter->first), pointer(&iter->second) { }

		const TKey& GetKey() const { return key; }
		const TValue& GetValue() const { if (pointer) return *pointer; return value; }
		TValue& GetValue() { if (pointer) return *pointer; return value; }
		void SetValue(const TValue& value) { this->value = value; if (pointer) *pointer = value; }
	};

	template<typename TKey, typename TValue>
	class Dictionary : public Object, public Collection<Pair<TKey, TValue> >, public IterableFast<DictionaryIterator<TKey, TValue> >
	{
	private:
		struct StlCompare {
			AutoPointer<ValueComparison<TKey> > comparison;
			StlCompare(ValueComparison<TKey>* comparison) : comparison(comparison) { }
			bool operator ()(TKey v1, TKey v2) const { return comparison->Compare(v1, v2) == ComparisonResult::Less; }
		};
		StlCompare keycomparison;
		AutoPointer<ValueComparison<TValue> > comparison;

		typename std::map<TKey, TValue, StlCompare> map;
		typedef Pair<TKey, TValue> mapitem;
		typedef Dictionary<TKey, TValue> dicttype;
		typedef DictionaryIterator<TKey, TValue> dictiter;
		typedef typename std::map<TKey, TValue, StlCompare>::iterator iterator;
		typedef typename std::map<TKey, TValue, StlCompare>::const_iterator const_iterator;

		friend class DictionaryIterator<TKey, TValue>;

		iterator IteratorOfValue(const TValue& value) {
			for (iterator iter = map.begin(); iter != map.end(); iter++) {
				if (!comparison->Compare(iter->second, value))
					return iter;
			}
			return map.end();
		}

		const_iterator IteratorOfValue(const TValue& value) const {
			for (const_iterator iter = map.begin(); iter != map.end(); iter++) {
				if (!comparison->Compare(iter->second, value))
					return iter;
			}
			return map.end();
		}

	public:
		Dictionary(ValueComparison<TKey>* keycomparison = autonew OperatorValueComparison<TKey>(), ValueComparison<TValue>* comparison = autonew OperatorValueComparison<TValue>()) : keycomparison(keycomparison), comparison(comparison), map(keycomparison) { }
		Dictionary(const Dictionary< TKey, TValue >& dictionary, ValueComparison<TKey>* keycomparison = NULL, ValueComparison<TValue>* comparison = NULL) : keycomparison(keycomparison ?: dictionary.keycomparison), comparison(comparison ?: dictionary.comparison.GetValue()), map(dictionary.map) { }
		Dictionary(Iterable<Pair<TKey, TValue> >* iterable, ValueComparison<TKey>* keycomparison = autonew OperatorValueComparison<TKey>(), ValueComparison<TValue>* comparison = autonew OperatorValueComparison<TValue>()) : keycomparison(keycomparison), comparison(comparison), map(keycomparison) { AddItems(iterable); }

		TValue& GetItem(const TKey& key, const TValue& value) { iterator iter = map.find(key); if (iter != map.end()) return iter->second; return map[key] = value; }
		TValue& GetItem(const TKey& key) { iterator iter = map.find(key); if (iter == map.end()) BRICKS_FEATURE_RELEASE_THROW(InvalidArgumentException()); return iter->second; }
		const TValue& GetItem(const TKey& key) const { const_iterator iter = map.find(key); if (iter == map.end()) BRICKS_FEATURE_RELEASE_THROW(InvalidArgumentException()); return iter->second; }
		ReturnPointer<Collection<const TKey&> > GetKeys() const;
		ReturnPointer<Collection<const TValue&> > GetValues() const;
		ReturnPointer<Collection<TValue&> > GetValues();

		bool ContainsKey(const TKey& key) const { return map.find(key) != map.end(); }
		bool ContainsValue(const TValue& value) const { return IteratorOfValue(value) != map.end(); }

		void Add(const TKey& key, const TValue& value) { map[key] = value; }
		void Set(const TKey& key, const TValue& value) { iterator iter = map.find(key); if (iter == map.end()) BRICKS_FEATURE_RELEASE_THROW(InvalidArgumentException()); iter->second = value; }
		bool RemoveKey(const TKey& key) { return map.erase(key); }
		bool RemoveValue(const TValue& value) { iterator iter = IteratorOfValue(value); if (iter == map.end()) return false; map.erase(iter); return true; }

		// Collections
		virtual long GetCount() const { return map.size(); }

		virtual bool ContainsItem(const Pair< TKey, TValue >& value) const { const_iterator iter = map.find(value.GetKey()); return iter != map.end() && !comparison->Compare(iter->second, value.GetValue()); }

		virtual void AddItem(const Pair< TKey, TValue >& value) { map[value.GetKey()] = value.GetValue(); }
		virtual void AddItems(Iterable<Pair<TKey, TValue> >* values) { foreach (const mapitem& value, values) AddItem(value); }
		virtual bool RemoveItem(const Pair<TKey, TValue>& value) { return map.erase(value.GetKey()); }
		virtual void Clear() { map.clear(); }

		// Iterator
		virtual ReturnPointer<Iterator<Pair<TKey, TValue> > > GetIterator() const { return autonew dictiter(const_cast<dicttype&>(*this)); }
		dictiter GetIteratorFast() const { return dictiter(const_cast<dicttype&>(*this)); }

		virtual TValue& operator[](const TKey& key) { return GetItem(key); }
		virtual const TValue& operator[](const TKey& key) const { return GetItem(key); }
	};

	template<typename TKey, typename TValue>
	class DictionaryIterator : public Iterator<Pair<TKey, TValue> >
	{
	private:
		bool first;
		typename std::map<TKey, TValue>::iterator position;
		typename std::map<TKey, TValue>::iterator end;
		mutable Pair<TKey, TValue> current;

		friend class Dictionary<TKey, TValue>;

	public:
		DictionaryIterator(Dictionary<TKey, TValue>& dictionary) : first(true), position(dictionary.map.begin()), end(dictionary.map.end()) { }
		Pair<TKey, TValue>& GetCurrent() const { return const_cast<Pair<TKey, TValue>&>(current = Pair<TKey, TValue>(position)); }
		bool MoveNext() { return (first ? first = false, position : ++position) != end; }
	};
} }
