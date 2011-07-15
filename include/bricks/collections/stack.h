#pragma once

#ifndef BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/collections.h"
#include "bricks/collections/comparison.h"

#include <deque>

namespace Bricks { namespace Collections {
	class StackEmptyException : public Exception
	{
	public:
		StackEmptyException(const String& message = String::Empty) : Exception(message) { }
	};

	template<typename T, typename V> class StackIterator;

	template<typename T, typename V = T>
	class Stack : public Collection< T >
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
		Stack(const Pointer< ValueComparison< T > >& comparison = TempAlloc<OperatorEqualityComparison< T > >()) : comparison(comparison) { }
		Stack(const Stack< T, V >& stack, const Pointer< ValueComparison< T > >& comparison = TempAlloc<OperatorEqualityComparison< T > >()) : comparison(comparison ?: stack.comparison), stack(stack.stack) { }
		Stack(const Collection< T >& collection, const Pointer< ValueComparison< T > >& comparison = TempAlloc<OperatorEqualityComparison< T > >()) : comparison(comparison) { AddItems(collection); }
		
		virtual ~Stack() { }

		void Push(const T& value) { stack.push_front(value); }
		void Push(T& value) { stack.push_front(value); }
		void Pop() { if (stack.empty()) throw StackEmptyException(); stack.pop_front(); }
		T PopItem() { if (stack.empty()) throw StackEmptyException(); T value = stack.front(); stack.pop_front(); return value; }
		T& Peek() { if (stack.empty()) throw StackEmptyException(); return stack.front(); }
		const T& Peek() const { if (stack.empty()) throw StackEmptyException(); return stack.front(); }

		// Iterator
		virtual Iterator< T >& GetIterator() const { return AutoAlloc<StackIterator< T, V > >(const_cast<Stack< T, V >&>(*this)); }

		// Collection
		virtual long GetCount() const { return stack.size(); };

		virtual bool ContainsItem(const T& value) const { return IteratorOfItem(value) != stack.end(); }

		virtual void AddItem(const T& value) { Push(value); }
		virtual void AddItem(T& value) { Push(value); }
		virtual void AddItems(const Iterable< T >& values) { foreach (const T& item, values) AddItem(item); }
		virtual void Clear() { stack.clear(); }
		virtual bool RemoveItem(const T& value) {
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
		bool first;
		typename Stack< T, V >::iterator position;
		typename Stack< T, V >::iterator end;

		friend class Stack< T, V >;

	public:
		StackIterator(Stack< T, V >& stack) : first(false), position(stack.stack.begin()), end(stack.stack.end()) { }
		virtual T& GetCurrent() const { if (!first || position >= end) throw InvalidIteratorException(); return *position; }
		virtual bool MoveNext() { if (!first) return (first = true) && position < end; return ++position < end; }
	};
} }
