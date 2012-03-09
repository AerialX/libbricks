#pragma once

namespace Bricks { class Timespan; class Time; }

namespace Bricks { namespace Threading {
	class Mutex;

	class MutexLock
	{
	protected:
		Mutex* mutex;
		bool isLocked;

	public:
		MutexLock(Mutex* mutex, bool tryLock = false);
		MutexLock(Mutex* mutex, const Timespan& timeout);
		MutexLock(Mutex* mutex, const Time& timeout);
		~MutexLock();

		void Unlock();

		operator bool() const { return isLocked; }

		bool IsLocked() const { return isLocked; }
		Mutex* GetMutex() const { return mutex; }
	};
} }
