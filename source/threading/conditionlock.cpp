#include "bricks/threading/conditionlock.h"

#include <pthread.h>

#define BRICKS_PTHREAD_COND CastToRaw<pthread_cond_t>(condHandle)
#define BRICKS_PTHREAD_MUTEX CastToRaw<pthread_cond_t>(mutexHandle)

namespace Bricks { namespace Threading {
	namespace Internal {
		struct ConditionLockCondition
		{
			const ConditionLock* lock;
			int target;

			ConditionLockCondition(const ConditionLock* lock, int target) : lock(lock), target(target) { }

			bool operator()() const { return lock->GetCondition() == target; }
		};
	}

	ConditionLock::ConditionLock(int condition) :
		condition(condition)
	{

	}

	ConditionLock::~ConditionLock()
	{

	}

	void ConditionLock::Lock(int targetCondition)
	{
		Condition::Lock(Internal::ConditionLockCondition(this, targetCondition));
	}

	bool ConditionLock::Lock(int targetCondition, const Time& timeout)
	{
		return Condition::Lock(Internal::ConditionLockCondition(this, targetCondition), timeout.GetTimespec());
	}

	bool ConditionLock::TryLock(int targetCondition)
	{
		return Condition::TryLock(Internal::ConditionLockCondition(this, targetCondition));
	}

	void ConditionLock::Unlock(int targetCondition)
	{
		Broadcast(targetCondition);
		Unlock();
	}

	void ConditionLock::Signal(int targetCondition)
	{
		condition = targetCondition;
		Signal();
	}

	void ConditionLock::Broadcast(int targetCondition)
	{
		condition = targetCondition;
		Broadcast();
	}
} }
