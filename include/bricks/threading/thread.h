#pragma once

#include "bricks/threading.h"

namespace Bricks { namespace Threading {
	namespace Internal {
		typedef long ThreadID;
		ThreadID GetCurrentThreadID();
	}
} }
