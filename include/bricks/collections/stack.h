#pragma once

#include "bricks/collections/collection.h"

#ifndef BRICKS_CONFIG_STL

namespace Bricks { namespace Collections {
	class StackEmptyException : public Exception
	{
	public:
		StackEmptyException(String& message = String::Empty) : Exception(message) { }
	};

	template<typename T> class StackIterator;

	template<typename T>
	class Stack : public Object, public Collection< T >
	{
	private:
		struct StackNode
		{
			StackNode(const T& data, StackNode* next) : data(data), next(next) { }
			T data;
			StackNode* next;
		};

		int count;
		StackNode* front;

		friend class StackIterator< T >;
		
	public:
		Stack() : Object(), count(0), front(NULL) { }
		Stack(const Stack< T >& stack) : Object(), count(0), front(NULL) { AddItems(stack); }
		Stack(const Collection< T >& collection) : Object(), count(0), front(NULL) { AddItems(collection); }
		virtual ~Stack() { Clear(); }

		void Push(const T& value) { front = new StackNode(value, front); count++; }
		void Pop() {
			if (!count)
				Throw(StackEmptyException);
			StackNode* node = front;
			front = node->next;
			delete node;
			count--;
		}
		T PopItem() {
			if (!count)
				Throw(StackEmptyException);
			StackNode* node = front;
			T value = node->data;
			front = node->next;
			delete node;
			count--;
			return value;
		}
		T& Peek() { if (!count) Throw(StackEmptyException); return front->data; }
		const T& Peek() const { if (!count) Throw(StackEmptyException); return front->data; }

		// Iterator
		virtual Iterator< T >& GetIterator() const { return AutoAlloc(StackIterator< T >, self); }

		// Collection
		virtual long GetCount() const { return count; };

		virtual bool ContainsItem(const T& value) const { foreach (T& item, self) if (item == value) return true; return false; }

		virtual void AddItem(const T& value) { Push(value); }
		virtual void AddItems(Iterable< T >& values) { foreach (T& item, values) AddItem(item); }
		virtual void AddItems(Collection< T >& values) { AddItems(static_cast<Iterable< T >&>(values)); }
		virtual void Clear() { while (count) Pop(); }
		virtual bool RemoveItem(const T& value) {
			StackNode* previous = NULL;
			for (StackNode* node = front; node; node = node->next) {
				if (node->data == value) {
					if (previous)
						previous->next = node->next;
					else
						front = node->next;
					delete node;
					count--;
					return true;
				}
				previous = node;
			}
			return false;
		}
	};
	
	template<typename T>
	class StackIterator : public Object, public Iterator< T >
	{
	private:
		Pointer< const Stack< T > > stack;
		typename Stack< T >::StackNode beginning;
		typename Stack< T >::StackNode* position;
		StackIterator(const Stack< T >& stack) : stack(stack), beginning(T(), stack.front), position(&beginning) { }

		friend class Stack< T >;

	public:
		virtual T& GetCurrent() const { if (!position || position == &beginning) Throw(InvalidIteratorException); return position->data; }
		virtual bool MoveNext() { return (position = position->next); }
	};
} }

#else

#include "bricks/collections/stack_stl.h"

#endif
