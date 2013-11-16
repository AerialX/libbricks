#pragma once

#include "bricks/config.h"

#if !BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/core/object.h"
#include "bricks/core/autopointer.h"
#include "bricks/core/exception.h"
#include "bricks/collections/comparison.h"
#include "bricks/collections/collection.h"

#include <deque>

namespace Bricks { namespace Collections {
	class QueueEmptyException : public Exception
	{
	public:
		QueueEmptyException(const String& message = String::Empty) : Exception(message) { }
	};

	template<typename T> class DequeIterator;

	template<typename T>
	class Deque : public Object, public Collection<T>, public IterableFast<DequeIterator<T> >
	{
	protected:
		AutoPointer<ValueComparison<T> > comparison;
		typename std::deque<T> queue;
		typedef typename std::deque<T>::iterator iterator;
		typedef typename std::deque<T>::const_iterator const_iterator;

		friend class DequeIterator<T>;

		const_iterator IteratorOfItem(const T& value) const {
			for (const_iterator iter = queue.begin(); iter != queue.end(); iter++) {
				if (!comparison->Compare(*iter, value))
					return iter;
			}
			return queue.end();
		}
		
		iterator IteratorOfItem(const T& value) {
			for (iterator iter = queue.begin(); iter != queue.end(); iter++) {
				if (!comparison->Compare(*iter, value))
					return iter;
			}
			return queue.end();
		}
		
	public:
		Deque(ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : comparison(comparison) { }
		Deque(const Deque<T>& queue, ValueComparison<T>* comparison = autonew OperatorValueComparison< T >()) : comparison(comparison ?: queue.comparison.GetValue()), queue(queue.queue) { }
		Deque(Iterable<T>* iterable, ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : comparison(comparison) { AddItems(iterable); }

		virtual void Push(const T& value) = 0;

		// Iterator
		virtual ReturnPointer<Iterator<T> > GetIterator() const { return autonew DequeIterator<T>(const_cast<Deque<T>&>(*this)); }
		DequeIterator<T> GetIteratorFast() const { return DequeIterator<T>(const_cast<Deque<T>&>(*this)); }

		// Collection
		virtual long GetCount() const { return queue.size(); };

		virtual bool ContainsItem(const T& value) const { return IteratorOfItem(value) != queue.end(); }

		virtual void AddItem(const T& value) { Push(value); }
		virtual void AddItems(Iterable<T>* values) { BRICKS_FOR_EACH (const T& item, values) AddItem(item); }
		virtual void Clear() { queue.clear(); }
		virtual bool RemoveItem(const T& value) {
			iterator iter = IteratorOfItem(value);
			if (iter == queue.end())
				return false;
			queue.erase(iter);
			return true;
		}
	};

	template<typename T>
	class DequeIterator : public Iterator<T>
	{
	private:
		typename Deque<T>::iterator position;
		typename Deque<T>::iterator end;

		friend class Deque<T>;

	public:
		DequeIterator(Deque<T>& queue) : position(queue.queue.begin() - 1), end(queue.queue.end()) { }
		T& GetCurrent() const { return *position; }
		bool MoveNext() { return ++position < end; }
	};
} }
