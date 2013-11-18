#pragma once

#include "bricks/core/object.h"
#include "bricks/core/delegate.h"
#include "bricks/core/time.h"
#include "bricks/core/copypointer.h"
#include "bricks/core/returnpointer.h"

namespace Bricks { namespace Threading {
	class Thread;
	class Mutex;

	namespace Internal {
		class TaskBase : public Object, NoCopy
		{
		protected:
			AutoPointer<Mutex> mutex;
			Thread* thread;

			virtual void Main() = 0;

		public:
			TaskBase();
			~TaskBase() { }

			void Run();

			void Wait(bool);
			bool TryWait();

			Thread* GetThread() const { return thread; }
			void SetThread(Thread* value) { thread = value; }
		};
	}

	template<typename T>
	class Task : public Internal::TaskBase
	{
	public:
		typedef Delegate<T()> TaskDelegate;

	protected:
		TaskDelegate delegate;
		typename TaskDelegate::ReturnType retValue;

		void Main() { retValue = delegate(); }

	public:
		Task() { }
		Task(const TaskDelegate& delegate) : delegate(delegate) { }
		~Task() { }

		void SetDelegate(const TaskDelegate& value) { delegate = value; }
		const TaskDelegate& GetDelegate() const { return delegate; }

		typename TaskDelegate::ReturnType Wait() { TaskBase::Wait(true); return retValue; }
		bool TryWait(typename TaskDelegate::ReturnType* ret) { if (!TryWait()) return false; *ret = retValue; return true; }
	};
} }
