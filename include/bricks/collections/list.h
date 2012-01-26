#pragma once

#include "bricks/collections/collection.h"

namespace Bricks { namespace Collections {
	template<typename T>
	class List : public Collection< T >
	{
	public:
		virtual void SetItem(long index, const T& value) = 0;
		virtual const T& GetItem(long index) const = 0;
		virtual T& GetItem(long index) = 0;
		virtual long IndexOfItem(const T& value) const = 0;

		virtual void InsertItem(long index, const T& value) = 0;
		virtual void RemoveItemAt(long index) = 0;

		virtual const T& operator[](long index) const { return GetItem(index); }
		virtual T& operator[](long index) { return GetItem(index); }
	};
} }
