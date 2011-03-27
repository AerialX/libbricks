#pragma once

#ifndef BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/collections.h"

#include <vector>

namespace Bricks { namespace Collections {
	template<typename T> class Array;

	template<typename T>
	class ArrayIterator : public Object, public Iterator< T >
	{
	private:
		bool first;
		typename Array< T >::iterator position;
		typename Array< T >::iterator end;
		ArrayIterator(Array< T >& array) : first(false), position(array.vector.begin()), end(array.vector.end()) { }

		friend class Array< T >;

	public:
		virtual T& GetCurrent() const { if (!first || position >= end) Throw(InvalidIteratorException); return *position; }
		virtual bool MoveNext() { if (!first) return (first = true) && position < end; return ++position < end; }
	};

	template<typename T>
	class Array : public Object, public List< T >
	{
	private:
		typename std::vector<T> vector;
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;

		friend class ArrayIterator< T >;

	public:
		Array() : Object() { }
		Array(const Array< T >& array) : Object(), vector(array.vector) { }
		Array(const Collection< T >& collection) : Object() { AddItems(collection); }
		virtual ~Array() { }

		// Iterator
		virtual Iterator< T >& GetIterator() const { return AutoAlloc(ArrayIterator< T >, const_cast<Array< T >&>(self)); }

		// Collection
		virtual long GetCount() const { return vector.size(); };

		virtual bool ContainsItem(const T& value) const { return IteratorOfItem(value) != vector.end(); }

		virtual void AddItem(const T& value) { vector.push_back(value); }
		virtual void AddItems(Iterable< T >& values) { foreach (T& item, values) AddItem(item); }
		virtual void AddItems(Collection< T >& values) { AddItems(static_cast<Iterable< T >&>(values)); }
		virtual bool RemoveItem(const T& value)
		{
			iterator iter = IteratorOfItem(value);
			if (iter == vector.end())
				return false;
			vector.erase(iter);
			return true;
		}

		virtual void Clear() { vector.clear(); }

		// List
		virtual void SetItem(long index, const T& value) { vector[index] = value; }
		virtual T& GetItem(long index) { return vector[index]; }
		virtual const T& GetItem(long index) const { return vector[index]; }
		virtual long IndexOfItem(const T& value) const {
			const_iterator iter = IteratorOfItem(value);
			if (iter == vector.end())
				return -1;
			return iter - vector.begin();
		}

		virtual void InsertItem(long index, const T& value)
		{
			vector.insert(vector.begin() + index, value);
		}

		virtual void RemoveItemAt(long index)
		{
			vector.erase(vector.begin() + index);
		}

		iterator IteratorOfItem(const T& value) {
			for (iterator iter = vector.begin(); iter != vector.end(); iter++) {
				if (*iter == value)
					return iter;
			}
			return vector.end();
		}
		
		const_iterator IteratorOfItem(const T& value) const {
			for (const_iterator iter = vector.begin(); iter != vector.end(); iter++) {
				if (*iter == value)
					return iter;
			}
			return vector.end();
		}
	};
} }
