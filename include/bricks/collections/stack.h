#pragma once

#ifndef BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/object.h"
#include "bricks/collections/comparison.h"
#include "bricks/collections/collection.h"

#include <deque>

namespace Bricks { namespace Collections {
	class StackEmptyException : public Exception
	{
	public:
		StackEmptyException(const String& message = String::Empty) : Exception(message) { }
	};

	template<typename T, typename V> class StackIterator;

	template<typename T, typename V = T>
	class Stack : public Object, public Collection< T >, public IterableFast<StackIterator<T, V> >
	{
	private:
		AutoPointer< ValueComparison< T > > comparison;
		typename std::deque< V > stack;
		typedef typename std::deque< V >::iterator iterator;
		typedef typename std::deque< V >::const_iterator const_iterator;

		friend class StackIterator< T, V >;

		const_iterator IteratorOfItem(const T& value) const {
			for (const_iterator iter = stack.begin(); iter != stack.end(); iter++) {
				if (!comparison->Compare(*iter, value))
					return iter;
			}
			return stack.end();
		}
		
		iterator IteratorOfItem(const T& value) {
			for (iterator iter = stack.begin(); iter != stack.end(); iter++) {
				if (!comparison->Compare(*iter, value))
					return iter;
			}
			return stack.end();
		}
		
	public:
		Stack(const Pointer< ValueComparison< T > >& comparison = autonew OperatorValueComparison< T >()) : comparison(comparison) { }
		Stack(const Stack< T, V >& stack, const Pointer< ValueComparison< T > >& comparison = autonew OperatorValueComparison< T >()) : comparison(comparison ?: stack.comparison), stack(stack.stack) { }
		Stack(const Collection< T >& collection, const Pointer< ValueComparison< T > >& comparison = autonew OperatorValueComparison< T >()) : comparison(comparison) { AddItems(collection); }

		void Push(const T& value) { stack.push_front(value); }
		void Push(T& value) { stack.push_front(value); }
		void Pop() { if (stack.empty()) BRICKS_FEATURE_RELEASE_THROW(StackEmptyException()); stack.pop_front(); }
		T PopItem() { if (stack.empty()) BRICKS_FEATURE_RELEASE_THROW(StackEmptyException()); T value = stack.front(); stack.pop_front(); return value; }
		T& Peek() { if (stack.empty()) BRICKS_FEATURE_RELEASE_THROW(StackEmptyException()); return stack.front(); }
		const T& Peek() const { if (stack.empty()) BRICKS_FEATURE_RELEASE_THROW(StackEmptyException()); return stack.front(); }

		// Iterator
		ReturnPointer< Iterator< T > > GetIterator() const { return autonew StackIterator< T, V >(const_cast<Stack< T, V >&>(*this)); }
		StackIterator<T, V> GetIteratorFast() const { return StackIterator<T, V>(const_cast<Stack<T, V>&>(*this)); }

		// Collection
		long GetCount() const { return stack.size(); };

		 bool ContainsItem(const T& value) const { return IteratorOfItem(value) != stack.end(); }

		void AddItem(const T& value) { Push(value); }
		void AddItem(T& value) { Push(value); }
		void AddItems(const Iterable< T >& values) { foreach (const T& item, values) AddItem(item); }
		void Clear() { stack.clear(); }
		bool RemoveItem(const T& value) {
			iterator iter = IteratorOfItem(value);
			if (iter == stack.end())
				return false;
			stack.erase(iter);
			return true;
		}
	};
	
	template<typename T, typename V>
	class StackIterator : public Iterator< T >
	{
	private:
		typename Stack< T, V >::iterator position;
		typename Stack< T, V >::iterator end;

		friend class Stack< T, V >;

	public:
		StackIterator(Stack< T, V >& stack) : position(stack.stack.begin() - 1), end(stack.stack.end()) { }
		T& GetCurrent() const { return *position; }
		bool MoveNext() { return ++position < end; }
	};
} }
