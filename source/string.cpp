#include "bricks/string.h"

#include <stdarg.h>
#include <stdio.h>

namespace Bricks {
	String String::Format(const char* format, ...)
	{
		size_t size = strlen(format) * 4;
		char* temp = new char[size];
		va_list args;
		va_start(args, format);
		vsnprintf(temp, size, format, args);
		va_end(args);
		temp[size - 1] = '\0';
		String ret(temp);
		delete[] temp;
		return ret;
	}
}
