#pragma once

#ifndef BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/collections/list.h"
#include "bricks/collections/comparison.h"

#include <vector>

namespace Bricks { namespace Collections {
	template<typename T, typename V> class Array;

	template<typename T, typename V>
	class ArrayIterator : public Iterator< T >
	{
	private:
		typename Array< T, V >::iterator position;
		typename Array< T, V >::iterator end;

		friend class Array< T, V >;

	public:
		ArrayIterator(Array< T, V >& array) : position(array.vector.begin() - 1), end(array.vector.end()) { }
		T& GetCurrent() const { return *position; }
		bool MoveNext() { return ++position < end; }
	};

	template<typename T, typename V = T>
	class Array : public Object, public List< T >, public IterableFast<ArrayIterator<T, V> >
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
		Array(const Pointer< ValueComparison< T > >& comparison = autonew OperatorValueComparison< T >()) : comparison(comparison) { }
		Array(const Array< T, V >& array, const Pointer< ValueComparison< T > >& comparison = NULL) : comparison(comparison ?: array.comparison), vector(array.vector) { }
		Array(const Collection< T >& collection, const Pointer< ValueComparison< T > >& comparison = autonew OperatorValueComparison< T >()) : comparison(comparison) { AddItems(collection); }

		// Iterator
		ReturnPointer< Iterator< T > > GetIterator() const { return autonew ArrayIterator< T, V >(const_cast<Array< T, V >&>(*this)); }
		ArrayIterator<T, V> GetIteratorFast() const { return ArrayIterator<T, V>(const_cast<Array<T, V>&>(*this)); }

		// Collection
		long GetCount() const { return vector.size(); };

		bool ContainsItem(const T& value) const { return IteratorOfItem(value) != vector.end(); }

		void AddItem(T& value) { vector.push_back(value); }
		void AddItem(const T& value) { vector.push_back(value); }
		void AddItems(const Iterable< T >& values) { foreach (const T& item, values) AddItem(item); }
		bool RemoveItem(const T& value)
		{
			iterator iter = IteratorOfItem(value);
			if (iter == vector.end())
				return false;
			vector.erase(iter);
			return true;
		}

		void Clear() { vector.clear(); }

		// List
		void SetItem(long index, const T& value) { vector[index] = value; }
		void SetItem(long index, T& value) { vector[index] = value; }
		const T& GetItem(long index) const { return vector[index]; }
		T& GetItem(long index) { return vector[index]; }
		long IndexOfItem(const T& value) const {
			const_iterator iter = IteratorOfItem(value);
			if (iter == vector.end())
				return -1;
			return iter - vector.begin();
		}

		void InsertItem(long index, const T& value) { vector.insert(vector.begin() + index, value); }
		void InsertItem(long index, T& value) { vector.insert(vector.begin() + index, value); }
		void RemoveItemAt(long index) { vector.erase(vector.begin() + index); }
	};
} }
