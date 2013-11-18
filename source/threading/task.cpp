#include "bricks/threading/task.h"
#include "bricks/threading/mutex.h"
#include "bricks/threading/mutexlock.h"

namespace Bricks { namespace Threading { namespace Internal {
	TaskBase::TaskBase() :
		mutex(autonew Mutex(MutexType::Lock)), thread(NULL)
	{
		mutex->Lock();
	}

	void TaskBase::Wait(bool)
	{
		MutexLock lock(mutex);
	}

	bool TaskBase::TryWait()
	{
		return MutexLock(mutex, true).IsLocked();
	}

	void TaskBase::Run()
	{
		Main();
		mutex->Unlock();
	}
} } }
