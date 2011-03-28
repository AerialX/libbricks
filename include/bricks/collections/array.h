#pragma once

#include "bricks/collections.h"

#ifndef BRICKS_CONFIG_STL

namespace Bricks { namespace Collections {
	template<typename T> class Array;

	template<typename T>
	class ArrayIterator : public Object, public Iterator< T >
	{
	private:
		Pointer< const Array< T > > array;
		T* position;
		T* end;
		ArrayIterator(const Array< T >& array) : array(array), position(array.data - 1), end(array.data + array.count) { }

		friend class Array< T >;

	public:
		virtual T& GetCurrent() const { if (position < array->data || position >= end) Throw(InvalidIteratorException); return *position; }
		virtual bool MoveNext() { return ++position < end; }
	};

	template<typename T>
	class Array : public Object, public List< T >
	{
	private:
		long allocated;
		long count;
		T* data;

		friend class ArrayIterator< T >;

		void Allocate(long size) {
			if (allocated >= size)
				return;
			allocated = size;
			T* old = data;
			data = new T[allocated];
			for (long i = 0; old && i < count; i++)
				data[i] = old[i];
			if (old)
				delete[] old;
		}
		
		void Shift(long source, long dest, long size)
		{
			for (long i = 0; i < size; i++) {
				data[dest + i] = data[source + i];
				data[source + i] = T();
			}
		}
		void ShiftReverse(long source, long dest, long size)
		{
			for (long i = size - 1; i >= 0; i--) {
				data[dest + i] = data[source + i];
				data[source + i] = T();
			}
		}

	public:
		Array() : allocated(0), count(0), data(NULL) { }
		Array(const Array< T >& array) : allocated(0), count(0), data(NULL) { AddItems(array); }
		Array(const Collection< T >& collection) : allocated(0), count(0), data(NULL) { AddItems(collection); }
		virtual ~Array() { if (data) delete[] data; }

		// Iterator
		virtual Iterator< T >& GetIterator() const { return AutoAlloc(ArrayIterator< T >, self); }

		// Collection
		virtual long GetCount() const { return count; };

		virtual bool ContainsItem(const T& value) const { return IndexOfItem(value) >= 0; }

		virtual void AddItem(const T& value) { Allocate(count + 1); data[count++] = value; }
		virtual void AddItems(Iterable< T >& values) { foreach (T& item, values) AddItem(item); }
		virtual void AddItems(Collection< T >& values) {
			Allocate(count + values.GetCount());
			foreach (T& item, values) {
				data[count] = item;
				count++;
			}
		}
		virtual bool RemoveItem(const T& value)
		{
			long index = IndexOfItem(value);
			if (index < 0)
				return false;
			RemoveItemAt(index);
			return true;
		}

		virtual void Clear() { foreach (T& item, self) item = T(); count = 0; }

		// List
		virtual void SetItem(long index, const T& value) { data[index] = value; }
		virtual T& GetItem(long index) { return data[index]; }
		virtual const T& GetItem(long index) const { return data[index]; }
		virtual long IndexOfItem(const T& value) const { for (long i = 0; i < count; i++) { if (data[i] == value) return i; } return -1; }

		virtual void InsertItem(long index, const T& value)
		{
			Allocate(count + 1);
			ShiftReverse(index, index + 1, count - index);
			count++;
		}

		virtual void RemoveItemAt(long index)
		{
			Shift(index + 1, index, count - index - 1);
			count--;
		}
	};
} }

#else

#include "bricks/collections/array_stl.h"

#endif
