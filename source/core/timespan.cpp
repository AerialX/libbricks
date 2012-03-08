#include "bricks/core/timespan.h"

#include <time.h>
#include <string.h>

#define BRICKS_TIMESPAN_CONVERSION_DAYS (864000000000LL)
#define BRICKS_TIMESPAN_CONVERSION_HOURS (36000000000LL)
#define BRICKS_TIMESPAN_CONVERSION_MINUTES (600000000LL)
#define BRICKS_TIMESPAN_CONVERSION_SECONDS 10000000
#define BRICKS_TIMESPAN_CONVERSION_MILLISECONDS (10000)
#define BRICKS_TIMESPAN_CONVERSION_MICROSECONDS (10)
#define BRICKS_TIMESPAN_CONVERSION_NANOSECONDS (100)

namespace Bricks {
	Timespan::Timespan(int hours, int minutes, int seconds) :
		ticks(hours * BRICKS_TIMESPAN_CONVERSION_HOURS + minutes * BRICKS_TIMESPAN_CONVERSION_MINUTES + seconds * BRICKS_TIMESPAN_CONVERSION_SECONDS)
	{

	}

	Timespan::Timespan(int hours, int minutes, int seconds, int milliseconds) :
		ticks(hours * BRICKS_TIMESPAN_CONVERSION_HOURS + minutes * BRICKS_TIMESPAN_CONVERSION_MINUTES + seconds * BRICKS_TIMESPAN_CONVERSION_SECONDS + milliseconds * BRICKS_TIMESPAN_CONVERSION_MILLISECONDS)
	{

	}

	Timespan::Timespan(int days, int hours, int minutes, int seconds, int milliseconds) :
		ticks(days * BRICKS_TIMESPAN_CONVERSION_DAYS + hours * BRICKS_TIMESPAN_CONVERSION_HOURS + minutes * BRICKS_TIMESPAN_CONVERSION_MINUTES + seconds * BRICKS_TIMESPAN_CONVERSION_SECONDS + milliseconds * BRICKS_TIMESPAN_CONVERSION_MILLISECONDS)
	{

	}

	Timespan Timespan::FromDays(long days)
	{
		return Timespan(days * BRICKS_TIMESPAN_CONVERSION_DAYS);
	}

	Timespan Timespan::FromDays(float days)
	{
		return Timespan(days * BRICKS_TIMESPAN_CONVERSION_DAYS);
	}

	Timespan Timespan::FromHours(long hours)
	{
		return Timespan(hours * BRICKS_TIMESPAN_CONVERSION_HOURS);
	}

	Timespan Timespan::FromHours(float hours)
	{
		return Timespan(hours * BRICKS_TIMESPAN_CONVERSION_HOURS);
	}

	Timespan Timespan::FromMinutes(long minutes)
	{
		return Timespan(minutes * BRICKS_TIMESPAN_CONVERSION_MINUTES);
	}

	Timespan Timespan::FromMinutes(float minutes)
	{
		return Timespan(minutes * BRICKS_TIMESPAN_CONVERSION_MINUTES);
	}

	Timespan Timespan::FromSeconds(s64 seconds)
	{
		return Timespan(seconds * BRICKS_TIMESPAN_CONVERSION_SECONDS);
	}

	Timespan Timespan::FromSeconds(float seconds)
	{
		return Timespan(seconds * BRICKS_TIMESPAN_CONVERSION_SECONDS);
	}

	Timespan Timespan::FromMilliseconds(s64 milliseconds)
	{
		return Timespan(milliseconds * BRICKS_TIMESPAN_CONVERSION_MILLISECONDS);
	}

	Timespan Timespan::FromMilliseconds(float milliseconds)
	{
		return Timespan(milliseconds * BRICKS_TIMESPAN_CONVERSION_MILLISECONDS);
	}

	s64 Timespan::ConvertDays(long days) { return days * BRICKS_TIMESPAN_CONVERSION_DAYS; }
	s64 Timespan::ConvertHours(long hours) { return hours * BRICKS_TIMESPAN_CONVERSION_HOURS; }
	s64 Timespan::ConvertMinutes(long minutes) { return minutes * BRICKS_TIMESPAN_CONVERSION_MINUTES; }
	s64 Timespan::ConvertSeconds(s64 seconds) { return seconds * BRICKS_TIMESPAN_CONVERSION_SECONDS; }
	s64 Timespan::ConvertMilliseconds(s64 milliseconds) { return milliseconds * BRICKS_TIMESPAN_CONVERSION_MILLISECONDS; }
	s64 Timespan::ConvertMicroseconds(s64 microseconds) { return microseconds * BRICKS_TIMESPAN_CONVERSION_MICROSECONDS; }
	s64 Timespan::ConvertNanoseconds(s64 nanoseconds) { return nanoseconds / BRICKS_TIMESPAN_CONVERSION_NANOSECONDS; }

	long Timespan::ConvertToDays(s64 ticks) { return ticks / BRICKS_TIMESPAN_CONVERSION_DAYS; }
	long Timespan::ConvertToHours(s64 ticks) { return ticks / BRICKS_TIMESPAN_CONVERSION_HOURS; }
	long Timespan::ConvertToMinutes(s64 ticks) { return ticks / BRICKS_TIMESPAN_CONVERSION_MINUTES; }
	s64 Timespan::ConvertToSeconds(s64 ticks) { return ticks / BRICKS_TIMESPAN_CONVERSION_SECONDS; }
	s64 Timespan::ConvertToMilliseconds(s64 ticks) { return ticks / BRICKS_TIMESPAN_CONVERSION_MILLISECONDS; }
	s64 Timespan::ConvertToMicroseconds(s64 ticks) { return ticks / BRICKS_TIMESPAN_CONVERSION_MICROSECONDS; }
	s64 Timespan::ConvertToNanoseconds(s64 ticks) { return ticks * BRICKS_TIMESPAN_CONVERSION_NANOSECONDS; }

	int Timespan::GetDays() const
	{
		return ticks / BRICKS_TIMESPAN_CONVERSION_DAYS;
	}

	int Timespan::GetHours() const
	{
		return AsHours() % 24;
	}

	int Timespan::GetMinutes() const
	{
		return AsMinutes() % 60;
	}

	int Timespan::GetSeconds() const
	{
		return AsSeconds() % 60;
	}

	int Timespan::GetMilliseconds() const
	{
		return AsMilliseconds() % 60;
	}

	int Timespan::AsDays() const
	{
		return GetDays();
	}

	int Timespan::AsHours() const
	{
		return ticks / BRICKS_TIMESPAN_CONVERSION_HOURS;
	}

	int Timespan::AsMinutes() const
	{
		return ticks / BRICKS_TIMESPAN_CONVERSION_MINUTES;
	}

	s64 Timespan::AsSeconds() const
	{
		return ticks / BRICKS_TIMESPAN_CONVERSION_SECONDS;
	}

	s64 Timespan::AsMilliseconds() const
	{
		return ticks / BRICKS_TIMESPAN_CONVERSION_MILLISECONDS;
	}

	float Timespan::GetTotalDays() const
	{
		return (float)ticks / BRICKS_TIMESPAN_CONVERSION_DAYS;
	}

	float Timespan::GetTotalHours() const
	{
		return (float)ticks / BRICKS_TIMESPAN_CONVERSION_HOURS;
	}

	float Timespan::GetTotalMinutes() const
	{
		return (float)ticks / BRICKS_TIMESPAN_CONVERSION_MINUTES;
	}

	float Timespan::GetTotalSeconds() const
	{
		return (float)ticks / BRICKS_TIMESPAN_CONVERSION_SECONDS;
	}

	float Timespan::GetTotalMilliseconds() const
	{
		return (float)ticks / BRICKS_TIMESPAN_CONVERSION_MILLISECONDS;
	}

	timespec Timespan::GetTimespec() const
	{
		struct timespec spec;
		memset(&spec, 0, sizeof(spec));
		spec.tv_sec = AsSeconds();
		spec.tv_nsec = (ticks - spec.tv_sec * BRICKS_TIMESPAN_CONVERSION_SECONDS) * BRICKS_TIMESPAN_CONVERSION_NANOSECONDS;
		return spec;
	}

	Timespan Timespan::operator +(const Timespan& span) const
	{
		return Timespan(*this) += span;
	}

	Timespan Timespan::operator -(const Timespan& span) const
	{
		return Timespan(*this) -= span;
	}

	Timespan& Timespan::operator +=(const Timespan& span)
	{
		ticks += span.ticks;
		return *this;
	}

	Timespan& Timespan::operator -=(const Timespan& span)
	{
		ticks -= span.ticks;
		return *this;
	}
}
