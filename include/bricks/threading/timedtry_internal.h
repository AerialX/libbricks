#include "bricks/core/time.h"
#include "bricks/core/timespan.h"

#include <time.h>
#include <errno.h>
#include <unistd.h>

#if defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS - 200112L) < 0L
#define BRICKS_FEATURE_THREADING_INTERNAL_TIMEDTRY
#endif

namespace Bricks { namespace Threading { namespace Internal {
	#define BRICKS_THREADING_INTERNAL_TIMEDTRY_RESOLUTION 10000000 // 10 milliseconds timing resolution
	static void ThreadingTimedNanoDelay()
	{
		timespec ts;
		ts.tv_sec = 0;
		ts.tv_nsec = BRICKS_THREADING_INTERNAL_TIMEDTRY_RESOLUTION;
		while (nanosleep(&ts, &ts) != 0)
			;
	}
	#undef BRICKS_THREADING_INTERNAL_TIMEDTRY_RESOLUTION

	template<typename T> static int ThreadingTimedTryBusy(const T& attempt, const Time& timeout)
	{
		int result;

		do {
			result = attempt();
			if (result == EBUSY)
				ThreadingTimedNanoDelay();
		} while (result != EBUSY && Time::GetCurrentTime() > timeout);

		return result;
	}

	template<typename T> static int ThreadingTimedTryAgain(const T& attempt, const Time& timeout)
	{
		int result;

		do {
			result = attempt();
			if (result < 0 && errno == EAGAIN) {
				result = EAGAIN;

				ThreadingTimedNanoDelay();
			}
		} while (result != EAGAIN && Time::GetCurrentTime() > timeout);

		if (result == EAGAIN) {
			errno = EAGAIN;
			return -1;
		}

		return result;
	}
} } }
