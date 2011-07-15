#pragma once

#ifndef BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/collections.h"
#include "bricks/collections/comparison.h"

#include <vector>

namespace Bricks { namespace Collections {
	template<typename T, typename V> class Array;

	template<typename T, typename V>
	class ArrayIterator : public Iterator< T >
	{
	private:
		bool first;
		typename Array< T, V >::iterator position;
		typename Array< T, V >::iterator end;
		ArrayIterator(Array< T, V >& array) : first(false), position(array.vector.begin()), end(array.vector.end()) { }

		friend class Array< T, V >;

	public:
		virtual T& GetCurrent() const { if (!first || position >= end) throw InvalidIteratorException(); return *position; }
		virtual bool MoveNext() { if (!first) return (first = true) && position < end; return ++position < end; }
	};

	template<typename T, typename V = T>
	class Array : public List< T >
	{
	private:
		AutoPointer< ValueComparison< T > > comparison;

		typename std::vector< V > vector;
		typedef typename std::vector< V >::iterator iterator;
		typedef typename std::vector< V >::const_iterator const_iterator;

		friend class ArrayIterator< T, V >;

		iterator IteratorOfItem(const T& value) {
			for (iterator iter = vector.begin(); iter != vector.end(); iter++) {
				if (!comparison->Compare(*iter, value))
					return iter;
			}
			return vector.end();
		}
		
		const_iterator IteratorOfItem(const T& value) const {
			for (const_iterator iter = vector.begin(); iter != vector.end(); iter++) {
				if (!comparison->Compare(*iter, value))
					return iter;
			}
			return vector.end();
		}

	public:
		Array(const Pointer< ValueComparison< T > >& comparison = TempAlloc<OperatorEqualityComparison< T > >()) : comparison(comparison) { }
		Array(const Array< T, V >& array, const Pointer< ValueComparison< T > >& comparison = NULL) : comparison(comparison ?: array.comparison), vector(array.vector) { }
		Array(const Collection< T >& collection, const Pointer< ValueComparison< T > >& comparison = TempAlloc<OperatorEqualityComparison< T > >()) : comparison(comparison) { AddItems(collection); }
		virtual ~Array() { }

		// Iterator
		virtual Iterator< T >& GetIterator() const { return AutoAlloc<ArrayIterator< T, V > >(const_cast<Array< T, V >&>(*this)); }

		// Collection
		virtual long GetCount() const { return vector.size(); };

		virtual bool ContainsItem(const T& value) const { return IteratorOfItem(value) != vector.end(); }

		template<typename T2> void AddItem(T2 value) { vector.push_back(value); }
		virtual void AddItem(T& value) { vector.push_back(value); }
		virtual void AddItem(const T& value) { vector.push_back(value); }
		virtual void AddItems(const Iterable< T >& values) { foreach (const T& item, values) AddItem(item); }
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
		virtual void SetItem(long index, T& value) { vector[index] = value; }
		virtual const T& GetItem(long index) const { return vector[index]; }
		virtual T& GetItem(long index) { return vector[index]; }
		virtual long IndexOfItem(const T& value) const {
			const_iterator iter = IteratorOfItem(value);
			if (iter == vector.end())
				return -1;
			return iter - vector.begin();
		}

		virtual void InsertItem(long index, const T& value) { vector.insert(vector.begin() + index, value); }
		virtual void InsertItem(long index, T& value) { vector.insert(vector.begin() + index, value); }
		virtual void RemoveItemAt(long index) { vector.erase(vector.begin() + index); }
	};
} }
