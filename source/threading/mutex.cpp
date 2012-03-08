#include "bricks/threading/mutex.h"

#include <pthread.h>

#define BRICKS_PTHREAD_MUTEX CastToRaw<pthread_mutex_t>(mutexHandle)

namespace Bricks { namespace Threading {
	Mutex::Mutex(MutexType::Enum type) :
		type(type)
	{
		pthread_mutexattr_t attributes;
		pthread_mutexattr_init(&attributes);
		pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_PRIVATE);
		int pthreadType;
		switch (type) {
			case MutexType::Lock:
				pthreadType = PTHREAD_MUTEX_NORMAL;
				break;
			case MutexType::RecursiveLock:
				pthreadType = PTHREAD_MUTEX_RECURSIVE;
				break;
			default:
				pthreadType = PTHREAD_MUTEX_DEFAULT;
				break;
		}
		pthread_mutexattr_settype(&attributes, pthreadType);

		mutexHandle = CastToRaw(new pthread_mutex_t());
		pthread_mutex_init(BRICKS_PTHREAD_MUTEX, &attributes);

		pthread_mutexattr_destroy(&attributes);
	}

	Mutex::~Mutex()
	{
		pthread_mutex_destroy(BRICKS_PTHREAD_MUTEX);
		delete BRICKS_PTHREAD_MUTEX;
	}

	void Mutex::Lock()
	{
		pthread_mutex_lock(BRICKS_PTHREAD_MUTEX);
	}

	bool Mutex::Lock(const Time& timeout)
	{
		return !pthread_mutex_timedlock(BRICKS_PTHREAD_MUTEX, tempnew timeout.GetTimespec());
	}

	bool Mutex::TryLock()
	{
		return !pthread_mutex_trylock(BRICKS_PTHREAD_MUTEX);
	}

	void Mutex::Unlock()
	{
		pthread_mutex_unlock(BRICKS_PTHREAD_MUTEX);
	}
} }
