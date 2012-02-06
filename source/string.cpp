#include "bricks/core/string.h"
#include "bricks/core/exception.h"

#include <stdarg.h>
#include <stdio.h>

namespace Bricks {
	const String String::Empty;

	String String::Format(const String& format, va_list args)
	{
#ifdef _GNU_SOURCE
		char* temp = NULL;
		if (vasprintf(&temp, format.CString(), args) < 0)
			BRICKS_FEATURE_THROW(OutOfMemoryException());
		String ret(temp);
		free(temp);
#else
		va_list argsCount;
		va_copy(argsCount, args);
		char dummy[1];
		int size = vsnprintf(dummy, 0, format.CString(), argsCount);
		va_end(argsCount);
		if (size < 0)
			BRICKS_FEATURE_THROW(NotSupportedException());
		char temp[size + 1];
		vsnprintf(temp, size + 1, format.CString(), args);
		temp[size] = '\0';
		String ret(temp);
#endif
		return ret;
	}

	String String::Format(const String& format, ...)
	{
		va_list args;
		va_start(args, format);
		String ret = Format(format, args);
		va_end(args);
		return ret;
	}

	int String::Scan(const String& format, va_list args)
	{
		return vsscanf(buffer, format.CString(), args);
	}

	int String::Scan(const String& format, ...)
	{
		va_list args;
		va_start(args, format);
		int ret = Scan(format, args);
		va_end(args);
		return ret;
	}
}
