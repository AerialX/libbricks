#pragma once

#include "bricks/core/types.h"

struct timespec;

namespace Bricks {
	class Timespan;

	class Time
	{
	protected:
		long seconds;
		long nanoseconds;

		void Normalize();

	public:
		Time() : seconds(0), nanoseconds(0) { }
		Time(long seconds, long nanoseconds = 0) : seconds(seconds), nanoseconds(nanoseconds) { }
		Time(const struct timespec& spec);

		static Time GetCurrentTime();

		long GetTime() const { return seconds; }
		long GetNanoseconds() const { return nanoseconds; }

		struct timespec GetTimespec() const;

		Time operator +(const Timespan& span) const;
		Time operator -(const Timespan& span) const;
		Time& operator +=(const Timespan& span);
		Time& operator -=(const Timespan& span);

		Timespan operator -(const Time& time) const;
	};
}
