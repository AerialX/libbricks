#include "bricks/exception.h"

#include <errno.h>

namespace Bricks {
	void ThrowErrno()
	{
		int err = errno;
		if (!err)
			return;
		switch (err) {
			case E2BIG:
				break;
			default:
				throw Exception(String::Format("errno: %d: %s", err, strerror(err)));
		}
	}
}
