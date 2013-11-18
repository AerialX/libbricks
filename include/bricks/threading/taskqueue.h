#pragma once

#include "bricks/core/object.h"
#include "bricks/core/copypointer.h"

namespace Bricks { namespace Collections { template<typename T> class Queue; template<typename T> class AutoArray; } }

namespace Bricks { namespace Threading {
	namespace Internal { class TaskBase; struct TaskQueueThread; }
	class ConditionLock;
	class Thread;

	namespace TaskQueueCondition { enum Enum {
		Waiting,
		Ready,
		Cancel
	}; }

	class TaskQueue : public Object, NoCopy
	{
	protected:
		typedef Collections::Queue<AutoPointer<Internal::TaskBase> > QueueType;
		typedef Collections::AutoArray<Thread> ThreadArray;

		int threadCount;
		AutoPointer<QueueType> queue;
		AutoPointer<ConditionLock> queueCondition;
		AutoPointer<ThreadArray> threads;

		void StartThread();

		static TaskQueueCondition::Enum ConditionStatus(const QueueType* queue);

		friend struct Internal::TaskQueueThread;

	public:
		TaskQueue(int threadCount = 0);
		~TaskQueue();

		void PushTask(Internal::TaskBase* task);

		void Start();
		void Stop(bool wait = false);
	};
} }
