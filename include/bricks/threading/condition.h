#pragma once

#include "bricks/threading/mutex.h"

namespace Bricks { namespace Threading {
	class Condition : public Mutex
	{
	protected:
		void* conditionHandle;

	public:
		Condition();
		~Condition();

		void Wait();
		bool Wait(const Timespan& timeout) { return Wait(Time::GetCurrentTime() + timeout); }
		bool Wait(const Time& timeout);

		void Signal();
		void Broadcast();

		using Mutex::Lock;
		using Mutex::TryLock;

		template<typename T> void Lock(const T& condition) {
			Lock();
			while (!condition())
				Wait();
		}

		template<typename T> bool Lock(const T& condition, const Timespan& timeout) { return Lock(condition, Time::GetCurrentTime() + timeout); }
		template<typename T> bool Lock(const T& condition, const Time& timeout) {
			if (!Lock(timeout))
				return false;
			while (!condition()) {
				if (!Wait(timeout)) {
					Unlock();
					return false;
				}
			}
			return true;
		}

		template<typename T> bool TryLock(const T& condition) {
			if (TryLock()) {
				if (condition())
					return true;
				Unlock();
			}
			return false;
		}
	};
} }
