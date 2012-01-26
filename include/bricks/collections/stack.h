#pragma once

#include "bricks/config.h"

#ifndef BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/core/object.h"
#include "bricks/core/autopointer.h"
#include "bricks/core/exception.h"
#include "bricks/collections/comparison.h"
#include "bricks/collections/collection.h"

#include <deque>

namespace Bricks { namespace Collections {
	class StackEmptyException : public Exception
	{
	public:
		StackEmptyException(const String& message = String::Empty) : Exception(message) { }
	};

	template<typename T> class StackIterator;

	template<typename T>
	class Stack : public Object, public Collection<T>, public IterableFast<StackIterator<T> >
	{
	private:
		AutoPointer<ValueComparison<T> > comparison;
		typename std::deque<T> stack;
		typedef typename std::deque<T>::iterator iterator;
		typedef typename std::deque<T>::const_iterator const_iterator;

		friend class StackIterator<T>;

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
		Stack(ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : comparison(comparison) { }
		Stack(const Stack<T>& stack, ValueComparison<T>* comparison = autonew OperatorValueComparison< T >()) : comparison(comparison ?: stack.comparison.GetValue()), stack(stack.stack) { }
		Stack(Iterable<T>* iterable, ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : comparison(comparison) { AddItems(iterable); }

		virtual void Push(const T& value) { stack.push_front(value); }
		virtual void Pop() { if (stack.empty()) BRICKS_FEATURE_RELEASE_THROW(StackEmptyException()); stack.pop_front(); }
		virtual T PopItem() { if (stack.empty()) BRICKS_FEATURE_RELEASE_THROW(StackEmptyException()); T value = stack.front(); stack.pop_front(); return value; }
		virtual T& Peek() { if (stack.empty()) BRICKS_FEATURE_RELEASE_THROW(StackEmptyException()); return stack.front(); }
		virtual const T& Peek() const { if (stack.empty()) BRICKS_FEATURE_RELEASE_THROW(StackEmptyException()); return stack.front(); }

		// Iterator
		virtual ReturnPointer<Iterator<T> > GetIterator() const { return autonew StackIterator<T>(const_cast<Stack<T>&>(*this)); }
		StackIterator<T> GetIteratorFast() const { return StackIterator<T>(const_cast<Stack<T>&>(*this)); }

		// Collection
		virtual long GetCount() const { return stack.size(); };

		virtual bool ContainsItem(const T& value) const { return IteratorOfItem(value) != stack.end(); }

		virtual void AddItem(const T& value) { Push(value); }
		virtual void AddItems(Iterable<T>* values) { foreach (const T& item, values) AddItem(item); }
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
	class StackIterator : public Iterator<T>
	{
	private:
		typename Stack<T>::iterator position;
		typename Stack<T>::iterator end;

		friend class Stack<T>;

	public:
		StackIterator(Stack<T>& stack) : position(stack.stack.begin() - 1), end(stack.stack.end()) { }
		T& GetCurrent() const { return *position; }
		bool MoveNext() { return ++position < end; }
	};
} }
