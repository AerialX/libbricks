#pragma once

#include "bricks/core/object.h"
#include "bricks/core/delegate.h"
#include "bricks/core/time.h"
#include "bricks/core/copypointer.h"
#include "bricks/core/returnpointer.h"

namespace Bricks { namespace Threading {
	class ConditionLock;

	namespace Internal {
		class ThreadID
		{
		public:
			void* handle;

			ThreadID();
			ThreadID(void*);
			ThreadID(const ThreadID&);
			~ThreadID();

			void SetHandle(void*);
			void* GetHandle();
		};
	}

	namespace ThreadStatus {
		enum Enum {
			None,
			Initialized,
			Started,
			Stopped
		};
	}

	class Thread : public Object, NoCopy
	{
	public:
		typedef Delegate<void()> ThreadDelegate;

	protected:
		Internal::ThreadID threadHandle;
		ThreadDelegate delegate;
		AutoPointer<ConditionLock> status;
		int stackSize;
		int priority;
		bool owned;

		static void* StaticMain(void* threadData);
		static void StaticCleanup(void* threadData);
		void Detach();
		void Cleanup();

		Thread(Thread* thread);

		ConditionLock* GetStatusLock() const { return status; }
		void SetOwned(bool value) { owned = value; }
		void SetThreadID(void* handle);

	public:
		Thread();
		Thread(const ThreadDelegate& delegate);
		~Thread();

		void SetDelegate(const ThreadDelegate& value) { delegate = value; }
		const ThreadDelegate& GetDelegate() const { return delegate; }

		void SetStackSize(int value);
		int GetStackSize() const { return stackSize; }

		void SetPriority(int value);
		int GetPriority() const { return priority; }

		virtual void Main();

		ThreadStatus::Enum GetStatus() const;
		bool OwnsThread() const { return owned; }

		void Start();
		void Stop();

		void Wait();
		bool Wait(const Timespan& timeout) { return Wait(Time::GetCurrentTime() + timeout); }
		bool Wait(const Time& timeout);
		bool TryWait();

		void Signal(int signal);

		static int GetHardwareConcurrency();
		static Thread* GetCurrentThread();

		static void Sleep(const Time& timeout);
		static void Sleep(const Timespan& timeout);

		static void Yield();
		static void YieldStop();
		static void Exit(void* result = NULL);
	};
} }
