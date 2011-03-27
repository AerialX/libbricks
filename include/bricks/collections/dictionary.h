#pragma once

#include "bricks/collections/collection.h"

namespace Bricks { namespace Collections {
	template<typename TKey, typename TValue>
	class Dictionary : public Collection< Pair<TKey, TValue> >
	{
	public:
		virtual TValue& GetItem(TKey& key) = 0;
		virtual Collection<TValue> GetValues() = 0;
		virtual Collection<TKey> GetKeys() = 0;

		virtual bool ContainsKey(TKey& key) = 0;
		virtual bool ContainsValue(TValue& value) = 0;

		virtual void Add(TKey& key, TValue& value) = 0;
		virtual bool RemoveKey(TKey& key) = 0;
		virtual bool RemoveValue(TValue& value) = 0;
	};
} }
