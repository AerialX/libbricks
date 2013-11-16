#pragma once

#include "bricks/config.h"

#if !BRICKS_CONFIG_STL
#error libbricks must be configured to use the STL
#endif

#include "bricks/collections/deque_internal.h"

namespace Bricks { namespace Collections {
	template<typename T>
	class Queue : public Deque<T>
	{
	public:
		Queue(ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : Deque<T>(comparison) { }
		Queue(const Queue<T>& queue, ValueComparison<T>* comparison = autonew OperatorValueComparison< T >()) : Deque<T>(queue, comparison) { }
		Queue(Iterable<T>* iterable, ValueComparison<T>* comparison = autonew OperatorValueComparison<T>()) : Deque<T>(iterable, comparison) { }

		virtual void Push(const T& value) { this->queue.push_back(value); }
		virtual void Pop() { if (this->queue.empty()) BRICKS_FEATURE_RELEASE_THROW(QueueEmptyException()); this->queue.pop_front(); }
		virtual T PopItem() { if (this->queue.empty()) BRICKS_FEATURE_RELEASE_THROW(QueueEmptyException()); T value = this->queue.front(); this->queue.pop_front(); return value; }
		virtual T& Peek() { if (this->queue.empty()) BRICKS_FEATURE_RELEASE_THROW(QueueEmptyException()); return this->queue.front(); }
		virtual const T& Peek() const { if (this->queue.empty()) BRICKS_FEATURE_RELEASE_THROW(QueueEmptyException()); return this->queue.front(); }
	};
} }
