#pragma once

#include "bricks/types.h"

#include <stdlib.h>
#include <string.h>

namespace Bricks {
	class String
	{
		public:
			static const size_t npos = -1;

			static String Format(const char* format, ...);

		protected:
			char* buffer;

			void Construct(const char* string, size_t len = npos)
			{
				if (buffer)
					free(buffer);
				if (len == npos)
					len = strlen(string);
				buffer = (char*)malloc(len + 1);
				if (buffer) {
					if (len)
						strncpy(buffer, string, len);
					buffer[len] = '\0';
				}
			}
		public:
			String() : buffer(NULL) { Construct(""); }
			String(const String& string) : buffer(NULL) { Construct(string.CString()); }
			String(const String& string, size_t off, size_t len = npos) : buffer(NULL) { Construct(string.CString() + off, len); }
			String(const char* string, size_t len = npos) : buffer(NULL) { Construct(string, len); }

			~String() { free(buffer); buffer = NULL; }

			String& operator =(const String& string)
			{
				Construct(string.CString(), string.Size());
				return *this;
			}
			String& operator =(const char* string)
			{
				Construct(string);
				return *this;
			}

			String operator +(const String& string) const
			{
				String out(*this, 0, Size() + string.Size());
				strcat(out.buffer, string.buffer);
				return out;
			}

			String& operator +=(const String& string)
			{
				char* newbuffer = (char*)malloc(Size() + string.Size());
				strcpy(newbuffer, buffer);
				strcat(newbuffer, string.buffer);
				free(buffer);
				buffer = newbuffer;
				return *this;
			}

			const char* CString() const { return buffer; }

			char& operator [](size_t index) { return buffer[index]; }

			size_t Size() const { return strlen(buffer); }
			size_t Length() const { return strlen(buffer); } // TODO: UTF8

			int Compare(const String& string) const { return strcmp(buffer, string.CString()); }
			int Compare(const String& string, size_t len) const { return strncmp(buffer, string.CString(), len); }
			int Compare(size_t off1, const String& string, size_t off2, size_t len) const { return strncmp(buffer + off1, string.CString() + off2, len); }
			int Compare(const char* string) const { return strcmp(buffer, string); }
			int Compare(const char* string, size_t len) const { return strncmp(buffer, string, len); }
			int Compare(size_t off1, const char* string, size_t len) const { return strncmp(buffer + off1, string, len); }

			String Substring(size_t off = 0, size_t len = npos) const { return String(buffer + off, len); }
	};
}
