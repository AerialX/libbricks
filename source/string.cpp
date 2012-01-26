#include "bricks/core/string.h"
#include "bricks/core/exception.h"

#include <stdarg.h>
#include <stdio.h>

namespace Bricks {
	const String String::Empty;

	String String::Format(const String& format, ...)
	{
		va_list args;
		va_start(args, format);
#ifdef _GNU_SOURCE
		char* temp = NULL;
		if (vasprintf(&temp, format.CString(), args) < 0)
			BRICKS_FEATURE_THROW(OutOfMemoryException());
		String ret(temp);
		free(temp);
#else
		char dummy[0];
		int size = vsnprintf(dummy, 0, format.CString(), args);
		va_end(args);
		if (size < 0)
			BRICKS_FEATURE_THROW(NotSupportedException());
		va_start(args, format);
		char temp[size + 1];
		vsnprintf(temp, size + 1, format.CString(), args);
		temp[size] = '\0';
		String ret(temp);
#endif
		va_end(args);
		return ret;
	}
}
