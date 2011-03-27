#pragma once

#include "bricks/collections/collection.h"

#ifndef BRICKS_CONFIG_STL

namespace Bricks { namespace Collections {
	template<typename TKey, typename TValue>
	class Dictionary : public Collection< Pair<TKey, TValue> >
	{
	public:
		TValue& GetItem(const TKey& key) = 0;
		const TValue& GetItem(const TKey& key) const = 0;
		const Collection<const TValue&>& GetValues() const = 0;
		const Collection<const TKey&>& GetKeys() const = 0;

		bool ContainsKey(const TKey& key) const = 0;
		bool ContainsValue(const TValue& value) const = 0;

		void Add(const TKey& key, const TValue& value) = 0;
		bool RemoveKey(const TKey& key) = 0;
		bool RemoveValue(const TValue& value) = 0;
		
		// Collections
		long GetCount() const = 0;

		bool ContainsItem(const T& value) const = 0;

		void AddItem(const T& value) = 0;
		void AddItems(Iterable< T >& values) = 0;
		void AddItems(Collection< T >& values) = 0;
		bool RemoveItem(const T& value) = 0;
		void Clear() = 0;
		
		// Iterator
		Iterator< T >& GetIterator() const = 0;
	};
} }

#else

#include "bricks/collections/dictionary_stl.h"

#endif
