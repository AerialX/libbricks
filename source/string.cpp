#include "bricks.h"

#include <stdarg.h>
#include <stdio.h>

namespace Bricks {
	const Pointer<String>& String::Empty = globalalloc String();

	String& String::Format(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
#ifdef _GNU_SOURCE
		char* temp = NULL;
		if (vasprintf(&temp, format, args) < 0)
			throw OutOfMemoryException();
		String& ret = autoalloc String(temp);
		free(temp);
#else
		size_t size = strlen(format) * 4;
		char* temp = new char[size];
		vsnprintf(temp, size, format, args);
		temp[size - 1] = '\0';
		String& ret = autoalloc String(temp);
		delete[] temp;
#endif
		va_end(args);
		return ret;
	}
}
