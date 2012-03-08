#include "bricks/threading/mutexlock.h"
#include "bricks/threading/mutex.h"

namespace Bricks { namespace Threading {
	MutexLock::MutexLock(Mutex* mutex, bool tryLock) :
		mutex(mutex), isLocked(tryLock ? mutex->TryLock() : (mutex->Lock(), true))
	{

	}

	MutexLock::MutexLock(Mutex* mutex, const Timespan& timeout) :
		mutex(mutex), isLocked(mutex->Lock(timeout))
	{

	}

	MutexLock::MutexLock(Mutex* mutex, const Time& timeout) :
		mutex(mutex), isLocked(mutex->Lock(timeout))
	{

	}

	MutexLock::~MutexLock()
	{
		Unlock();
	}

	void MutexLock::Unlock()
	{
		if (isLocked) {
			mutex->Unlock();
			isLocked = false;
		}
	}
} }
