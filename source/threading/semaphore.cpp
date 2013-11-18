#include "bricks/threading/semaphore.h"
#include "bricks/threading/mutex.h"
#include "bricks/threading/timedtry_internal.h"

#if BRICKS_ENV_MINGW
#include <pthread.h>
#endif

#include <semaphore.h>

#define BRICKS_SEMAPHORE CastToRaw<sem_t>(semaphoreHandle)

namespace Bricks { namespace Threading {
	Semaphore::Semaphore(int value)
	{
		semaphoreHandle = CastToRaw(new sem_t());
		sem_init(BRICKS_SEMAPHORE, 0, value);
	}

	Semaphore::~Semaphore()
	{
		sem_destroy(BRICKS_SEMAPHORE);
		delete BRICKS_SEMAPHORE;
	}

	int Semaphore::GetValue() const
	{
		int value;
		sem_getvalue(BRICKS_SEMAPHORE, &value);
		return value;
	}

	void Semaphore::Signal()
	{
		sem_post(BRICKS_SEMAPHORE);
	}

	void Semaphore::Wait()
	{
		sem_wait(BRICKS_SEMAPHORE);
	}

	namespace Internal {
		struct SemaphoreTimedTry {
			sem_t* semaphore;

			SemaphoreTimedTry(sem_t* semaphore) : semaphore(semaphore) { }

			int operator()() const { return sem_trywait(semaphore); }
		};
	}

	bool Semaphore::Wait(const Time& timeout)
	{
#if BRICKS_ENV_THREADING_INTERNAL_TIMEDTRY
		return !Internal::ThreadingTimedTryAgain(Internal::SemaphoreTimedTry(BRICKS_SEMAPHORE), timeout);
#else
		return !sem_timedwait(BRICKS_SEMAPHORE, tempnew timeout.GetTimespec());
#endif
	}

	bool Semaphore::TryWait()
	{
		return !sem_trywait(BRICKS_SEMAPHORE);
	}
} }
