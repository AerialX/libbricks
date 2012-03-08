#pragma once

#include "bricks/core/types.h"

struct timespec;

namespace Bricks {
	class Timespan
	{
	protected:
		s64 ticks;

	public:
		explicit Timespan(s64 ticks) : ticks(ticks) { }

		Timespan() : ticks(0) { }
		Timespan(int hours, int minutes, int seconds);
		Timespan(int hours, int minutes, int seconds, int milliseconds);
		Timespan(int days, int hours, int minutes, int seconds, int milliseconds);

		static Timespan FromDays(long days);
		static Timespan FromDays(float days);
		static Timespan FromHours(long hours);
		static Timespan FromHours(float hours);
		static Timespan FromMinutes(long minutes);
		static Timespan FromMinutes(float minutes);
		static Timespan FromSeconds(s64 seconds);
		static Timespan FromSeconds(float seconds);
		static Timespan FromMilliseconds(s64 milliseconds);
		static Timespan FromMilliseconds(float milliseconds);

		static s64 ConvertDays(long days);
		static s64 ConvertHours(long hours);
		static s64 ConvertMinutes(long minutes);
		static s64 ConvertSeconds(s64 seconds);
		static s64 ConvertMilliseconds(s64 milliseconds);
		static s64 ConvertMicroseconds(s64 microseconds);
		static s64 ConvertNanoseconds(s64 nanoseconds);

		static long ConvertToDays(s64 ticks);
		static long ConvertToHours(s64 ticks);
		static long ConvertToMinutes(s64 ticks);
		static s64 ConvertToSeconds(s64 ticks);
		static s64 ConvertToMilliseconds(s64 ticks);
		static s64 ConvertToMicroseconds(s64 ticks);
		static s64 ConvertToNanoseconds(s64 ticks);

		s64 GetTicks() const { return ticks; }
		operator s64() const { return ticks; }

		int GetDays() const;
		int GetHours() const;
		int GetMinutes() const;
		int GetSeconds() const;
		int GetMilliseconds() const;

		int AsDays() const;
		int AsHours() const;
		int AsMinutes() const;
		s64 AsSeconds() const;
		s64 AsMilliseconds() const;

		float GetTotalDays() const;
		float GetTotalHours() const;
		float GetTotalMinutes() const;
		float GetTotalSeconds() const;
		float GetTotalMilliseconds() const;

		struct timespec GetTimespec() const;

		Timespan operator +(const Timespan& span) const;
		Timespan operator -(const Timespan& span) const;
		Timespan& operator +=(const Timespan& span);
		Timespan& operator -=(const Timespan& span);

#define BRICKS_TIMESPAN_COMPARISON(op) \
		bool operator op(const Timespan& rhs) const { return ticks op rhs.ticks; }
		BRICKS_TIMESPAN_COMPARISON(==);
		BRICKS_TIMESPAN_COMPARISON(!=);
		BRICKS_TIMESPAN_COMPARISON(>=);
		BRICKS_TIMESPAN_COMPARISON(<=);
		BRICKS_TIMESPAN_COMPARISON(>);
		BRICKS_TIMESPAN_COMPARISON(<);
#undef BRICKS_TIMESPAN_COMPARISON
	};
}
