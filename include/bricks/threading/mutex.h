#pragma once

#include "bricks/core/object.h"
#include "bricks/core/time.h"
#include "bricks/core/copypointer.h"

namespace Bricks { namespace Threading {
	namespace MutexType {
		enum Enum {
			Lock,
			RecursiveLock
		};
	}

	class Mutex : public Object, NoCopy
	{
	protected:
		MutexType::Enum type;
		void* mutexHandle;

	public:
		Mutex(MutexType::Enum type = MutexType::Lock);
		~Mutex();

		MutexType::Enum GetType() const { return type; }

		void Lock();
		bool Lock(const Timespan& timeout) { return Lock(Time::GetCurrentTime() + timeout); }
		bool Lock(const Time& timeout);
		bool TryLock();
		void Unlock();
	};
} }
