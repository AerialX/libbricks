#include "brickstest.hpp"

#include <bricks/threading/threadlocalstorage.h>
#include <bricks/threading/thread.h>
#include <bricks/threading/conditionlock.h>
#include <bricks/threading/monitor.h>
#include <bricks/threading/taskqueue.h>
#include <bricks/threading/task.h>
#include <bricks/core/timespan.h>
#include <bricks/core/value.h>

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
	EXPECT_EQ(1, functor.value);
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
		EXPECT_EQ(3, lock.GetCondition());
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

struct BricksThreadingThreadTestLocalStorageThread
{
	AutoPointer<ThreadLocalStorage<String> > tls;
	BricksThreadingThreadTestLocalStorageThread(ThreadLocalStorage<String>* tls) : tls(tls) { }
	void operator()()
	{
		EXPECT_FALSE(tls->HasValue());
		tls->SetValue("thread");
		EXPECT_TRUE(tls->HasValue());
		EXPECT_EQ("thread", tls->GetValue());
	}
};

TEST(BricksThreadingThreadTest, LocalStorage) {
	ThreadLocalStorage<String> tls;
	EXPECT_FALSE(tls.HasValue());
	tls.SetValue("mainthread");
	EXPECT_TRUE(tls.HasValue());
	BricksThreadingThreadTestLocalStorageThread functor(tempnew tls);
	Thread thread(tempnew functor);
	thread.Start();
	thread.Wait();
	EXPECT_EQ("mainthread", tls.GetValue());
}

struct BricksThreadingThreadTestAutoLocalStorageThread
{
	AutoPointer<AutoThreadLocalStorage<String> > tls;
	BricksThreadingThreadTestAutoLocalStorageThread(AutoThreadLocalStorage<String>* tls) : tls(tls) { }
	void operator()()
	{
		EXPECT_FALSE(tls->HasValue());
		tls->SetValue(autonew String("thread"));
		EXPECT_TRUE(tls->HasValue());
		EXPECT_EQ("thread", *tls->GetValue());
	}
};

TEST(BricksThreadingThreadTest, AutoLocalStorage) {
	AutoThreadLocalStorage<String> tls;
	EXPECT_FALSE(tls.HasValue());
	tls.SetValue(autonew String("mainthread"));
	EXPECT_TRUE(tls.HasValue());
	BricksThreadingThreadTestAutoLocalStorageThread functor(tempnew tls);
	Thread thread(tempnew functor);
	thread.Start();
	thread.Wait();
	EXPECT_EQ("mainthread", *tls.GetValue());
}

struct BricksThreadingThreadTestMonitorThread
{
	Monitor<Value> monitor;
	BricksThreadingThreadTestMonitorThread(const Monitor<Value>& monitor) : monitor(monitor) { }
	void operator()() { for (int i = 0; i < 100000; i++) monitor->SetValue(monitor->GetInt32Value() + 1); }
};

TEST(BricksThreadingThreadTest, Monitor) {
	AutoMonitor<Value> monitor = autonew Value((s32)0);
	BricksThreadingThreadTestMonitorThread functor(monitor);
	Thread thread(tempnew functor);
	thread.Start();
	for (int i = 0; i < 100000; i++)
		monitor->SetValue(monitor->GetInt32Value() + 1);
	thread.Wait();
	EXPECT_EQ(200000, monitor->GetInt32Value());
}

struct BricksThreadingThreadTestTaskThread
{
	Monitor<Value> monitor;
	int expected;
	BricksThreadingThreadTestTaskThread(const Monitor<Value>& monitor, int expected = -1) : monitor(monitor), expected(expected) { }
	int operator()() { if (expected >= 0) EXPECT_EQ(expected, monitor->GetInt32Value()); for (int i = 0; i < 100; i++) monitor->SetValue(monitor->GetInt32Value() + 1); return monitor->GetInt32Value(); }
};

TEST(BricksThreadingThreadTest, TaskSync) {
	AutoMonitor<Value> monitor = autonew Value((s32)0);

	AutoPointer<TaskQueue> queue = autonew TaskQueue(1); // 1 thread, serial queue

	for (int i = 0; i < 3; i++)
		queue->PushTask(autonew Task<int>(BricksThreadingThreadTestTaskThread(monitor, i * 100)));

	AutoPointer<Task<int> > task = autonew Task<int>(BricksThreadingThreadTestTaskThread(monitor, 300));
	queue->PushTask(task);

	queue->Start();

	EXPECT_EQ(400, task->Wait());
}

TEST(BricksThreadingThreadTest, TaskAsync) {
	AutoMonitor<Value> monitor = autonew Value((s32)0);

	AutoPointer<TaskQueue> queue = autonew TaskQueue(3); // 3 threads
	for (int i = 0; i < 10; i++)
		queue->PushTask(autonew Task<int>(BricksThreadingThreadTestTaskThread(monitor)));

	queue->Start();

	queue->Stop(true); // Waits for queue to finish
	EXPECT_EQ(1000, monitor->GetInt32Value());
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
