#include "bricks/threading/condition.h"

#include <pthread.h>

#define BRICKS_PTHREAD_COND CastToRaw<pthread_cond_t>(conditionHandle)
#define BRICKS_PTHREAD_MUTEX CastToRaw<pthread_mutex_t>(mutexHandle)

namespace Bricks { namespace Threading {
	Condition::Condition() :
		Mutex(MutexType::Lock)
	{
		pthread_condattr_t attributes;
#if !BRICKS_ENV_ANDROID
		pthread_condattr_init(&attributes);
		pthread_condattr_setpshared(&attributes, PTHREAD_PROCESS_PRIVATE);
#endif

		conditionHandle = CastToRaw(new pthread_cond_t());
		pthread_cond_init(BRICKS_PTHREAD_COND, &attributes);

#if !BRICKS_ENV_ANDROID
		pthread_condattr_destroy(&attributes);
#endif
	}

	Condition::~Condition()
	{
		pthread_cond_destroy(BRICKS_PTHREAD_COND);
		delete BRICKS_PTHREAD_COND;
	}

	void Condition::Wait()
	{
		pthread_cond_wait(BRICKS_PTHREAD_COND, BRICKS_PTHREAD_MUTEX);
	}

	bool Condition::Wait(const Time& timeout)
	{
		return !pthread_cond_timedwait(BRICKS_PTHREAD_COND, BRICKS_PTHREAD_MUTEX, tempnew timeout.GetTimespec());
	}

	void Condition::Signal()
	{
		pthread_cond_signal(BRICKS_PTHREAD_COND);
	}

	void Condition::Broadcast()
	{
		pthread_cond_broadcast(BRICKS_PTHREAD_COND);
	}
} }
