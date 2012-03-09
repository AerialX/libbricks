#include "bricks/core/time.h"
#include "bricks/core/timespan.h"

#include <time.h>
#include <sys/time.h>
#include <string.h>

#define BRICKS_TIME_CONVERT_NANOSECONDS 1000000000
#define BRICKS_TIME_CONVERT_MICROSECONDS_NANOSECONDS 1000

namespace Bricks {
	Time::Time(const struct timespec& spec) :
		seconds(spec.tv_sec), nanoseconds(spec.tv_nsec)
	{

	}

	Time::Time(const struct timeval& val) :
		seconds(val.tv_sec), nanoseconds((long)val.tv_usec * BRICKS_TIME_CONVERT_MICROSECONDS_NANOSECONDS)
	{

	}

	Time Time::GetCurrentTime()
	{
#ifdef CLOCK_REALTIME
		struct timespec spec;
		clock_gettime(CLOCK_REALTIME, &spec);
		return Time(spec);
#else
		struct timeval val;
		gettimeofday(&val, NULL);
		return Time(val);
#endif
	}

	void Time::Normalize()
	{
		long secs = nanoseconds / BRICKS_TIME_CONVERT_NANOSECONDS;
		if (secs) {
			seconds += secs;
			nanoseconds %= BRICKS_TIME_CONVERT_NANOSECONDS;
		}
	}

	struct timespec Time::GetTimespec() const
	{
		struct timespec spec;
		memset(&spec, 0, sizeof(spec));
		spec.tv_sec = seconds;
		spec.tv_nsec = nanoseconds;
		return spec;
	}

	struct timeval Time::GetTimeval() const
	{
		struct timeval val;
		memset(&val, 0, sizeof(val));
		val.tv_sec = seconds;
		val.tv_usec = nanoseconds / BRICKS_TIME_CONVERT_MICROSECONDS_NANOSECONDS;
		return val;
	}

	Time Time::operator +(const Timespan& span) const
	{
		return Time(*this) += span;
	}

	Time Time::operator -(const Timespan& span) const
	{
		return Time(*this) -= span;
	}

	Time& Time::operator +=(const Timespan& span)
	{
		int secs = span.AsSeconds();
		seconds += secs;
		nanoseconds += Timespan::ConvertToNanoseconds(span.GetTicks() - Timespan::ConvertSeconds(secs));
		Normalize();
		return *this;
	}

	Time& Time::operator -=(const Timespan& span)
	{
		int secs = span.AsSeconds();
		seconds -= secs;
		nanoseconds -= Timespan::ConvertToNanoseconds(span.GetTicks() - Timespan::ConvertSeconds(secs));
		Normalize();
		return *this;
	}

	Timespan Time::operator -(const Time& time) const
	{
		return Timespan(Timespan::ConvertSeconds(seconds - time.seconds) + Timespan::ConvertNanoseconds(nanoseconds - time.nanoseconds));
	}
}
