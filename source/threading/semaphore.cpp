#include "bricks/threading/semaphore.h"
#include "bricks/threading/mutex.h"

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

	bool Semaphore::Wait(const Time& timeout)
	{
		return !sem_timedwait(BRICKS_SEMAPHORE, tempnew timeout.GetTimespec());
	}

	bool Semaphore::TryWait()
	{
		return !sem_trywait(BRICKS_SEMAPHORE);
	}
} }
