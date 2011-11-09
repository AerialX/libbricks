#include "bricksall.hpp"

#include <errno.h>

namespace Bricks {
	void ThrowErrno()
	{
		int err = errno;
		if (!err)
			return;
		switch (err) {
/*			case E2BIG:
				break;*/
			default:
				BRICKS_FEATURE_THROW(ErrnoException(String::Format("errno %d: %s", err, strerror(err)), err));
		}
	}
}
