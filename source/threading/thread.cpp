#include "bricks/threading/thread.h"
#include "bricks/threading/conditionlock.h"
#include "bricks/threading/threadlocalstorage.h"
#include "bricks/core/timespan.h"

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sched.h>

#define BRICKS_PTHREAD_THREAD_REF CastToRaw<pthread_t>(threadHandle)
#define BRICKS_PTHREAD_THREAD (*CastToRaw<pthread_t>(threadHandle))

namespace Bricks { namespace Threading {
	static AutoThreadLocalStorage<Thread> localThreads;

	Thread::Thread() :
		threadHandle(NULL),
		stackSize(0), priority(0),
		owned(true)
	{

	}

	Thread::Thread(Thread* thread) :
		threadHandle(NULL),
		delegate(thread->delegate),
		status(thread->GetStatusLock()),
		stackSize(thread->GetStackSize()), priority(thread->GetPriority()),
		owned(thread->OwnsThread())
	{
		if (thread->threadHandle)
			threadHandle = CastToRaw(new pthread_t(*CastToRaw<pthread_t>(thread->threadHandle)));
	}

	Thread::Thread(const ThreadDelegate& delegate) :
		threadHandle(NULL),
		delegate(delegate),
		stackSize(0), priority(0),
		owned(true)
	{

	}

	Thread::Thread(ThreadID thread) :
		stackSize(0), priority(0),
		owned(false)
	{
		threadHandle = CastToRaw(new pthread_t());
		BRICKS_PTHREAD_THREAD = (pthread_t)thread;
	}

	Thread::~Thread()
	{
		Detach();
	}

	void* Thread::StaticMain(void* threadData)
	{
		Thread* originalThread = CastToRaw<Thread>(threadData);
		AutoPointer<Thread> thread = autonew Thread(originalThread);
		originalThread->Release();
		localThreads.SetValue(thread);
//		thread->SetPriority(thread->GetPriority()); // TODO: Actually obey thread priority
		pthread_cleanup_push(&Thread::StaticCleanup, CastToRaw(thread));
		thread->Main();
		pthread_cleanup_pop(true);
		return NULL;
	}

	void Thread::StaticCleanup(void* threadData)
	{
		Thread* thread = CastToRaw<Thread>(threadData);
		thread->GetStatusLock()->Unlock(ThreadStatus::Stopped);
	}

	void Thread::Start()
	{
		if (threadHandle)
			BRICKS_FEATURE_THROW(InvalidOperationException());

		pthread_attr_t attributes;
		pthread_attr_init(&attributes);
		pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_JOINABLE);
		if (stackSize)
			pthread_attr_setstacksize(&attributes, stackSize);

		status = autonew ConditionLock(ThreadStatus::Started);
		status->Lock();
		Retain();
		threadHandle = CastToRaw(new pthread_t());
		if (pthread_create(BRICKS_PTHREAD_THREAD_REF, &attributes, &Thread::StaticMain, CastToRaw(this)))
			Release();

		pthread_attr_destroy(&attributes);
	}

	void Thread::Main()
	{
		delegate();
	}

	void Thread::Cleanup()
	{
		if (BRICKS_PTHREAD_THREAD_REF) {
			delete BRICKS_PTHREAD_THREAD_REF;
			threadHandle = NULL;
		}
	}

	void Thread::Stop()
	{
#if BRICKS_ENV_ANDROID
		BRICKS_FEATURE_THROW(NotSupportedException());
#else
		if (GetStatus() == ThreadStatus::Started)
			pthread_cancel(BRICKS_PTHREAD_THREAD);
#endif
	}

	void Thread::Detach()
	{
		if (OwnsThread() && GetStatus() == ThreadStatus::Started)
			pthread_detach(BRICKS_PTHREAD_THREAD);
		Cleanup();
	}

	void Thread::Wait()
	{
		ThreadStatus::Enum threadStatus = GetStatus();
		if (threadStatus == ThreadStatus::Stopped)
			return;
		if (threadStatus != ThreadStatus::Started)
			BRICKS_FEATURE_THROW(InvalidOperationException());
		if (pthread_join(BRICKS_PTHREAD_THREAD, NULL))
			BRICKS_FEATURE_THROW(Exception());
		Cleanup();
	}

	bool Thread::Wait(const Time& timeout)
	{
		if (!OwnsThread())
			BRICKS_FEATURE_THROW(NotSupportedException());
		if (status->Lock(ThreadStatus::Stopped, timeout)) {
			status->Unlock();
			Wait();
			return true;
		}
		return false;
	}

	bool Thread::TryWait()
	{
		if (!OwnsThread())
			BRICKS_FEATURE_THROW(NotSupportedException());
		if (status->TryLock(ThreadStatus::Stopped)) {
			status->Unlock();
			Wait();
			return true;
		}
		return false;
	}

	void Thread::Signal(int signal)
	{
		pthread_kill(BRICKS_PTHREAD_THREAD, signal);
	}

	void Thread::SetStackSize(int value)
	{
		if (GetStatus() != ThreadStatus::Initialized)
			BRICKS_FEATURE_THROW(InvalidOperationException());
		stackSize = value;
	}

	void Thread::SetPriority(int value)
	{
		priority = value;
		if (GetStatus() == ThreadStatus::Started) {
			int policy;
			struct sched_param params;
			pthread_getschedparam(BRICKS_PTHREAD_THREAD, &policy, &params);

			params.sched_priority = value;

			pthread_setschedparam(BRICKS_PTHREAD_THREAD, policy, &params);
		}
	}

	ThreadStatus::Enum Thread::GetStatus() const
	{
		if (OwnsThread())
			return !BRICKS_PTHREAD_THREAD_REF ? ThreadStatus::Initialized : (ThreadStatus::Enum)status->GetCondition();
		return !BRICKS_PTHREAD_THREAD_REF ? ThreadStatus::Stopped : ThreadStatus::Started;
	}

	void Thread::Exit(void* result)
	{
		pthread_exit(result);
	}

	void Thread::Yield()
	{
		sched_yield();
	}

	void Thread::YieldStop()
	{
#if !BRICKS_ENV_ANDROID
		pthread_testcancel();
#endif
	}

	void Thread::Sleep(const Time& timeout)
	{
		Sleep(timeout - Time::GetCurrentTime());
	}

	void Thread::Sleep(const Timespan& timeout)
	{
		int seconds = timeout.AsSeconds();
		if (seconds > 0)
			sleep(seconds);
		int microseconds = Timespan::ConvertToMicroseconds(timeout.GetTicks() - Timespan::ConvertSeconds(seconds));
		if (microseconds > 0)
			usleep(microseconds);
	}

	Thread* Thread::GetCurrentThread()
	{
		if (!localThreads.HasValue()) {
			AutoPointer<Thread> thread = autonew Thread((ThreadID)pthread_self());
			localThreads.SetValue(thread);
			return thread;
		}

		return localThreads.GetValue();
	}

	int Thread::GetHardwareConcurrency()
	{
#if BRICKS_CONFIG_CPP0X
		return std::thread::hardware_concurrency();
#elif BRICKS_ENV_WINDOWS
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		return sysinfo.dwNumberOfProcessors;
/*#elif BRICKS_ENV_APPLE
		return [[[NSProcessInfo processInfo] activeProcessorCount] intValue];*/
#else
		return sysconf(_SC_NPROCESSORS_ONLN);
#endif
	}

	namespace Internal {
		ThreadID::ThreadID() : handle(NULL) { }
		ThreadID::ThreadID(void* handle) : handle(new pthread_t()) { SetHandle(handle); }
		ThreadID::ThreadID(const ThreadID& threadID) : handle(new pthread_t()) { SetHandle(threadID.handle); }
		ThreadID::~ThreadID() { SetHandle(NULL); }

		void* ThreadID::GetHandle() { return handle; }

		void ThreadID::SetHandle(void* handle)
		{
			if (handle) {
				if (!this->handle)
					this->handle = CastToRaw(new pthread_t());
				*CastToRaw<pthread_t>(this->handle) = *CastToRaw<pthread_t>(handle);
			} else {
				if (this->handle)
					delete CastToRaw<pthread_t>(handle);
				this->handle = NULL;
			}
		}
	}
} }
