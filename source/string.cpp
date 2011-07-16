#include "bricksall.hpp"

#include <stdarg.h>
#include <stdio.h>

namespace Bricks {
	const Pointer<String>& String::Empty = GlobalAlloc<String>();

	String& String::Format(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
#ifdef _GNU_SOURCE
		char* temp = NULL;
		if (vasprintf(&temp, format, args) < 0)
			throw OutOfMemoryException();
		String& ret = AutoAlloc<String>(temp);
		free(temp);
#else
		int size = vsnprintf(NULL, 0, format, args);
		if (size < 0)
			throw NotSupportedException();
		char* temp = new char[size + 1];
		vsnprintf(temp, size + 1, format, args);
		temp[size] = '\0';
		String& ret = AutoAlloc<String>(temp);
		delete[] temp;
#endif
		va_end(args);
		return ret;
	}
}
