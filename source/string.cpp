#include "bricks.h"

#include <stdarg.h>
#include <stdio.h>

namespace Bricks {
	String String::Empty = String();

	String& String::Format(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
#ifdef _GNU_SOURCE
		char* temp = NULL;
		if (vasprintf(&temp, format, args) < 0)
			Throw(OutOfMemoryException);
		String& ret = AutoAlloc(String, temp);
		free(temp);
#else
		size_t size = strlen(format) * 4;
		char* temp = new char[size];
		vsnprintf(temp, size, format, args);
		temp[size - 1] = '\0';
		String& ret = AutoAlloc(String, temp);
		delete[] temp;
#endif
		va_end(args);
		return ret;
	}
}
