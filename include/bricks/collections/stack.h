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

	template<typename T> class StackIterator;

	template<typename T>
	class Stack : public Object, public Collection< T >
	{
	private:
		AutoPointer< ValueComparison< T > > comparison;
		typename std::deque< T > stack;
		typedef typename std::deque< T >::iterator iterator;
		typedef typename std::deque< T >::const_iterator const_iterator;

		friend class StackIterator< T >;

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
		Stack(Pointer< ValueComparison< T > > comparison = autoalloc OperatorEqualityComparison< T >()) : comparison(comparison) { }
		Stack(const Stack< T >& stack, Pointer< ValueComparison< T > > comparison = autoalloc OperatorEqualityComparison< T >()) : comparison(comparison), stack(stack.stack) { }
		Stack(const Collection< T >& collection, Pointer< ValueComparison< T > > comparison = autoalloc OperatorEqualityComparison< T >()) : comparison(comparison) { AddItems(collection); }
		
		virtual ~Stack() { }

		void Push(const T& value) { stack.push_front(value); }
		void Pop() { if (stack.empty()) throw StackEmptyException(); stack.pop_front(); }
		T PopItem() { if (stack.empty()) throw StackEmptyException(); T value = stack.front(); stack.pop_front(); return value; }
		T& Peek() { if (stack.empty()) throw StackEmptyException(); return stack.front(); }
		const T& Peek() const { if (stack.empty()) throw StackEmptyException(); return stack.front(); }

		// Iterator
		virtual Iterator< T >& GetIterator() const { return autoalloc StackIterator< T >(const_cast<Stack< T >&>(self)); }

		// Collection
		virtual long GetCount() const { return stack.size(); };

		virtual bool ContainsItem(const T& value) const { return IteratorOfItem(value) != stack.end(); }

		virtual void AddItem(const T& value) { Push(value); }
		virtual void AddItems(Iterable< T >& values) { foreach (T& item, values) AddItem(item); }
		virtual void AddItems(Collection< T >& values) { AddItems(static_cast<Iterable< T >&>(values)); }
		virtual void Clear() { stack.clear(); }
		virtual bool RemoveItem(const T& value) {
			iterator iter = IteratorOfItem(value);
			if (iter == stack.end())
				return false;
			stack.erase(iter);
			return true;
		}
	};
	
	template<typename T>
	class StackIterator : public Object, public Iterator< T >
	{
	private:
		bool first;
		typename Stack< T >::iterator position;
		typename Stack< T >::iterator end;
		StackIterator(Stack< T >& stack) : first(false), position(stack.stack.begin()), end(stack.stack.end()) { }

		friend class Stack< T >;

	public:
		virtual T& GetCurrent() const { if (!first || position >= end) throw InvalidIteratorException(); return *position; }
		virtual bool MoveNext() { if (!first) return (first = true) && position < end; return ++position < end; }
	};
} }
