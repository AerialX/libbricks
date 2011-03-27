#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace Bricks {
	class String : public Object, public CopyableConstructor<String>
	{
		public:
			static const size_t npos = -1;
			static String Empty;

			static String& Format(const char* format, ...);

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
					if (len && string)
						strncpy(buffer, string, len);
					buffer[len] = '\0';
				}
			}
		public:
			String() : buffer(NULL) { Construct(""); }
			String(const String& string, size_t off = 0, size_t len = npos) : buffer(NULL) { Construct(string.CString() + off, len); }
			String(const char* string, size_t len = npos) : buffer(NULL) { Construct(string, len); }
			String(char character, size_t repeat = 1) : buffer(NULL) { Construct(NULL, repeat); for (size_t i = 0; i < repeat; i++) buffer[i] = character; }

			~String() { free(buffer); buffer = NULL; }

			String& GetDebugString() const { return Format("\"%s\" [%d]", static_cast<const Copyable&>(self).Copy<String>().CString(), GetReferenceCount()); }

			String& operator =(const String& string)
			{
				if (this != &string) // TODO: Throw the CString into the autorelease pool instead.
					Construct(string.CString(), string.GetSize());
				return self;
			}
			String& operator =(const char* string)
			{
				Construct(string);
				return self;
			}

			String& operator +(const String& string) const
			{
				String& out = AutoAlloc(String, self, 0, GetSize() + string.GetSize());
				strcat(out.buffer, string.buffer);
				return out;
			}

			String& operator +=(const String& string)
			{
				char* newbuffer = (char*)malloc(GetSize() + string.GetSize() + 1);
				strcpy(newbuffer, buffer);
				strcat(newbuffer, string.buffer);
				free(buffer);
				buffer = newbuffer;
				return self;
			}

			const char* CString() const { return buffer; }

			char& operator [](size_t index) { return buffer[index]; }

			size_t GetSize() const { return strlen(buffer); }
			size_t GetLength() const { return strlen(buffer); } // TODO: UTF8

			int Compare(const String& string) const { return strcmp(buffer, string.CString()); }
			int Compare(const String& string, size_t len) const { return strncmp(buffer, string.CString(), len); }
			int Compare(size_t off1, const String& string, size_t off2, size_t len) const { return strncmp(buffer + off1, string.CString() + off2, len); }
			int Compare(const char* string) const { return strcmp(buffer, string); }
			int Compare(const char* string, size_t len) const { return strncmp(buffer, string, len); }
			int Compare(size_t off1, const char* string, size_t len) const { return strncmp(buffer + off1, string, len); }

			String& Substring(size_t off = 0, size_t len = npos) const { return AutoAlloc(String, buffer + off, len); }
	};
}
