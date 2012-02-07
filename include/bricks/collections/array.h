#pragma once

#include "bricks/config.h"

#ifndef BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/collections/list.h"
#include "bricks/collections/comparison.h"

#include <vector>
#include <algorithm>

namespace Bricks { namespace Collections {
	template<typename T> class Array;

	template<typename T>
	class ArrayIterator : public Iterator<T>
	{
	private:
		typename Array<T>::iterator position;
		typename Array<T>::iterator end;

		friend class Array<T>;

	public:
		ArrayIterator(Array<T>& array) : position(array.vector.begin() - 1), end(array.vector.end()) { }
		T& GetCurrent() const { return *position; }
		bool MoveNext() { return ++position < end; }
	};

	template<typename T>
	class Array : public Object, public List<T>, public IterableFast<ArrayIterator<T> >
	{
	private:
		AutoPointer<ValueComparison<T> > comparison;

		typename std::vector<T> vector;
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;

		friend class ArrayIterator<T>;

		struct StlCompare {
			AutoPointer<ValueComparison<T> > comparison;
			StlCompare(ValueComparison<T>* comparison) : comparison(comparison) { }
			bool operator ()(T v1, T v2) const { return comparison->Compare(v1, v2) == ComparisonResult::Less; }
		};

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
		Array(const Array<T>& array, ValueComparison<T>* comparison = NULL) : comparison(comparison ?: array.comparison.GetValue()), vector(array.vector) { }
		Array(ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : comparison(comparison) { }
		Array(Iterable<T>* iterable, ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : comparison(comparison) { AddItems(iterable); }

		// Iterator
		virtual ReturnPointer<Iterator<T> > GetIterator() const { return autonew ArrayIterator<T>(const_cast<Array<T>&>(*this)); }
		ArrayIterator<T> GetIteratorFast() const { return ArrayIterator<T>(const_cast<Array<T>&>(*this)); }

		// Collection
		virtual long GetCount() const { return vector.size(); }

		virtual bool ContainsItem(const T& value) const { return IteratorOfItem(value) != vector.end(); }

		virtual void AddItem(const T& value) { vector.push_back(value); }
		virtual void AddItems(Iterable<T>* values) { foreach (const T& item, values) AddItem(item); }
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
		virtual const T& GetItem(long index) const { return vector[index]; }
		virtual T& GetItem(long index) { return vector[index]; }
		virtual long IndexOfItem(const T& value) const {
			const_iterator iter = IteratorOfItem(value);
			if (iter == vector.end())
				return -1;
			return iter - vector.begin();
		}

		virtual void InsertItem(long index, const T& value) { vector.insert(vector.begin() + index, value); }
		virtual void RemoveItemAt(long index) { vector.erase(vector.begin() + index); }

		virtual void Sort(ValueComparison<T>* sortComparison = NULL)
		{
			std::sort(vector.begin(), vector.end(), StlCompare(sortComparison ?: comparison.GetValue()));
		}
	};
} }
