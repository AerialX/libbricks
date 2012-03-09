#pragma once

#include "bricks/core/object.h"
#include "bricks/core/time.h"
#include "bricks/core/copypointer.h"

namespace Bricks { namespace Threading {
	class Semaphore : public Object, NoCopy
	{
	protected:
		void* semaphoreHandle;

	public:
		Semaphore(int value);
		~Semaphore();

		int GetValue() const;

		void Signal();
		void Wait();
		bool Wait(const Timespan& timeout) { return Wait(Time::GetCurrentTime() - timeout); }
		bool Wait(const Time& timeout);
		bool TryWait();
	};
} }
