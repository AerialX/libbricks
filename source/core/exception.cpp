#include "bricks/core/exception.h"

#include <errno.h>

namespace Bricks {
	void ThrowErrno()
	{
		int err = errno;
		if (!err)
			return;
		BRICKS_FEATURE_THROW(ErrnoException(String::Format("errno %d: %s", err, strerror(err)), err));
	}

	void ThrowErrno(const String& message)
	{
		int err = errno;
		if (!err)
			return;
		BRICKS_FEATURE_THROW(ErrnoException(String::Format("errno %d: %s (%s)", err, strerror(err), message.CString()), err));
	}
}
