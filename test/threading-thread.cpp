#include "brickstest.hpp"

#include <bricks/threading/thread.h>
#include <bricks/threading/conditionlock.h>
#include <bricks/core/timespan.h>

using namespace Bricks;
using namespace Bricks::Threading;

struct BricksThreadingThreadTestBasicFunctor
{
	volatile int value;
	BricksThreadingThreadTestBasicFunctor() : value(0) { }
	void operator()() { value++; }
};

TEST(BricksThreadingThreadTest, Basic) {
	BricksThreadingThreadTestBasicFunctor functor;
	Thread thread(tempnew functor);
	thread.Start();
	thread.Wait();
	EXPECT_EQ(functor.value, 1);
}

struct BricksThreadingThreadTestConditionLockThread1
{
	ConditionLock* condition;
	BricksThreadingThreadTestConditionLockThread1(ConditionLock* condition) : condition(condition) { }

	void operator()() {
		condition->Lock(1);
		condition->Unlock(2);
	}
};

struct BricksThreadingThreadTestConditionLockThread2
{
	ConditionLock* condition;
	BricksThreadingThreadTestConditionLockThread2(ConditionLock* condition) : condition(condition) { }

	void operator()() {
		condition->Lock(2);
		condition->Unlock(3);
	}
};

TEST(BricksThreadingThreadTest, ConditionLock) {
	ConditionLock lock;
	BricksThreadingThreadTestConditionLockThread1 threadDelegate1(tempnew lock);
	BricksThreadingThreadTestConditionLockThread2 threadDelegate2(tempnew lock);
	Thread thread1(tempnew threadDelegate1);
	Thread thread2(tempnew threadDelegate2);

	thread1.Start();
	thread2.Start();

	Thread::Sleep(Timespan(0, 0, 0, 50));
	lock.Broadcast(1);

	if (!lock.Lock(3, Timespan(0, 0, 1))) {
		thread1.Stop();
		thread2.Stop();
		FAIL();
	} else {
		lock.Unlock();
		thread1.Wait();
		thread2.Wait();
		EXPECT_EQ(lock.GetCondition(), 3);
	}
}

struct BricksThreadingThreadTestConditionLockTimeoutThread
{
	ConditionLock* condition;
	BricksThreadingThreadTestConditionLockTimeoutThread(ConditionLock* condition) : condition(condition) { }

	void operator()() {
		Thread::Sleep(Timespan(0, 0, 30));
		condition->Lock(1);
		condition->Unlock(2);
	}
};

TEST(BricksThreadingThreadTest, ConditionLockTimeout) {
	ConditionLock lock;
	BricksThreadingThreadTestConditionLockTimeoutThread threadDelegate(tempnew lock);
	Thread thread(tempnew threadDelegate);

	thread.Start();

	Thread::Sleep(Timespan(0, 0, 0, 50));
	lock.Broadcast(1);

	if (!lock.Lock(2, Timespan(0, 0, 0, 100))) {
		thread.Stop();
		SUCCEED();
	} else {
		lock.Unlock();
		thread.Stop();
		FAIL();
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
