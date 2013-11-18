#include "bricks/threading/taskqueue.h"
#include "bricks/threading/conditionlock.h"
#include "bricks/threading/mutexlock.h"
#include "bricks/threading/thread.h"
#include "bricks/threading/task.h"
#include "bricks/collections/autoarray.h"
#include "bricks/collections/queue.h"

namespace Bricks { namespace Threading {
	TaskQueue::TaskQueue(int threadCount) :
		threadCount(threadCount),
		queue(autonew QueueType()),
		queueCondition(autonew ConditionLock()),
		threads(autonew ThreadArray())
	{

	}

	TaskQueue::~TaskQueue()
	{
		Stop(false);
	}

	namespace Internal { struct TaskQueueThread
	{
		TaskQueue* queue;
		Thread* thread;

		TaskQueueThread(TaskQueue* queue, Thread* thread) : queue(queue), thread(thread) { }

		void operator()() const {
			AutoPointer<ConditionLock> condition = queue->queueCondition;
			AutoPointer<TaskQueue::QueueType> tasks = queue->queue;

			while (true) {
				MutexLock lock(condition);

				while (condition->GetCondition() == TaskQueueCondition::Waiting)
					condition->Wait();

				if (tasks->GetCount()) {
					AutoPointer<Internal::TaskBase> task = tasks->PopItem();
					condition->Signal(TaskQueue::ConditionStatus(tasks));
					lock.Unlock();

					task->SetThread(thread);
					task->Run();
				} else if (condition->GetCondition() == TaskQueueCondition::Cancel)
					break;
			}
		}
	}; }

	void TaskQueue::PushTask(Internal::TaskBase* task)
	{
		MutexLock lock(queueCondition);
		queue->Push(task);
		queueCondition->Signal(TaskQueueCondition::Ready);
	}

	void TaskQueue::StartThread()
	{
		AutoPointer<Thread> thread = autonew Thread();
		thread->SetDelegate(Internal::TaskQueueThread(this, thread));
		thread->Start();

		threads->AddItem(thread);
	}

	void TaskQueue::Start()
	{
		queueCondition->SetCondition(ConditionStatus(queue));

		if (!threadCount)
			threadCount = Thread::GetHardwareConcurrency();

		if (!threadCount)
			threadCount = 2;

		for (int i = 0; i < threadCount; i++)
			StartThread();
	}

	void TaskQueue::Stop(bool wait)
	{
		{	MutexLock lock(queueCondition);
			if (queueCondition->GetCondition() == TaskQueueCondition::Cancel)
				return;
			queueCondition->Signal(TaskQueueCondition::Cancel);
		}

		if (wait) {
			foreach (Thread* thread, threads)
				thread->Wait();
		}
	}

	TaskQueueCondition::Enum TaskQueue::ConditionStatus(const QueueType* queue)
	{
		return queue->GetCount() ? TaskQueueCondition::Ready : TaskQueueCondition::Cancel;
	}
} }
