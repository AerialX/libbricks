#pragma once

#include "bricks/collections/iterator.h"

namespace Bricks { namespace Collections {
	template<typename T>
	class Collection : public Iterable< T >
	{
	public:
		virtual long GetCount() const = 0;

		virtual bool ContainsItem(const T& value) const = 0;

		virtual void AddItem(const T& value) = 0;
		virtual void AddItems(Iterable<T>* values) { foreach (const T& value, values) AddItem(value); }
		virtual bool RemoveItem(const T& value) = 0;
		virtual void RemoveItems(const T& value) { while (RemoveItem(value)) ; }
		virtual void Clear() = 0;
	};
} }
