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
				throw ErrnoException(String::Format("errno %d: %s", err, strerror(err)), errno);
		}
	}
}
