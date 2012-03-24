#pragma once

#include "bricks/core/object.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#if BRICKS_ENV_OBJC
#include <Foundation/NSString.h>
#endif

namespace Bricks {
	class String : public Object
	{
		public:
			static const size_t npos = -1;
			static const String Empty;
			typedef char Character;

			static String FormatVariadic(const String& format, va_list args);
			static String Format(const String& format, ...);

			int ScanVariadic(const String& format, va_list args);
			int Scan(const String& format, ...);

		protected:
			char* buffer;

			void Construct(const char* string, size_t len = npos)
			{
				if (buffer)
					free(buffer);
				if (len == npos)
					len = string ? strlen(string) : 0;
				buffer = (char*)malloc(len + 1);
				if (buffer) {
					if (len && string)
						strncpy(buffer, string, len);
					buffer[len] = '\0';
				}
			}

			size_t ConvertStrChr(const char* ptr) const
			{
				if (!ptr)
					return npos;
				return ptr - buffer;
			}
		public:
			String() : buffer(NULL) { Construct(""); }
			String(const String& string, size_t off = 0, size_t len = npos) : buffer(NULL) { Construct(string.CString() + off, len); }
			String(const char* string, size_t len = npos) : buffer(NULL) { Construct(string, len); }
			String(char character, size_t repeat = 1) : buffer(NULL) { Construct(NULL, repeat); for (size_t i = 0; i < repeat; i++) buffer[i] = character; }
#if BRICKS_ENV_OBJC
			String(NSString* string, size_t len = npos) : buffer(NULL) { Construct([string UTF8String], len); }
#endif

			~String() { free(buffer); }

			String GetDebugString() const { return Format("\"%s\" [%d]", CString(), GetReferenceCount()); }

			String& operator =(const String& string)
			{
				if (this != &string)
					Construct(string.CString(), string.GetSize());
				return *this;
			}
			String& operator =(const char* string)
			{
				Construct(string);
				return *this;
			}

			String operator +(const String& string) const
			{
				String out(*this, 0, GetSize() + string.GetSize());
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
				return *this;
			}

			const char* CString() const { return buffer; }
			const void* GetBuffer() const { return buffer; }

			char& operator [](size_t index) { return buffer[index]; }
			const char& operator [](size_t index) const { return buffer[index]; }

			size_t GetSize() const { return strlen(buffer); }
			size_t GetLength() const { return strlen(buffer); } // TODO: UTF8

			int Compare(const String& string) const { return strcmp(buffer, string.CString()); }
			int Compare(const String& string, size_t len) const { return strncmp(buffer, string.CString(), len); }
			int Compare(size_t off1, const String& string, size_t off2, size_t len) const { return strncmp(buffer + off1, string.CString() + off2, len); }
			int Compare(const char* string) const { return strcmp(buffer, string); }
			int Compare(const char* string, size_t len) const { return strncmp(buffer, string, len); }
			int Compare(size_t off1, const char* string, size_t len) const { return strncmp(buffer + off1, string, len); }
			bool operator ==(const String& string) const { return !Compare(string); }
			bool operator >(const String& string) const { return Compare(string) > 0; }
			bool operator <(const String& string) const { return Compare(string) < 0; }

			String Substring(size_t off = 0, size_t len = npos) const { return String(buffer + (off == npos ? 0 : off), len); }

			size_t FirstIndexOf(char chr, size_t off = 0) const { return ConvertStrChr(strchr(CString() + off, chr)); }
			size_t LastIndexOf(char chr, size_t off = 0) const { return ConvertStrChr(strrchr(CString() + off, chr)); }
			
			size_t FirstIndexOf(const String& string, size_t off = 0) const {
				size_t count = string.GetSize();
				size_t firstindex = GetSize();
				for (size_t i = 0; i < count; i++) {
					size_t index = FirstIndexOf(string[i], off);
					if (index != npos && index < firstindex)
						firstindex = index;
				}
				return firstindex == GetSize() ? npos : firstindex;
			}
			size_t LastIndexOf(const String& string, size_t off = 0) const {
				size_t count = string.GetSize();
				size_t firstindex = 0;
				bool found = false;
				for (size_t i = 0; i < count; i++) {
					size_t index = LastIndexOf(string[i], off);
					if (index != npos && index > firstindex) {
						firstindex = index;
						found = true;
					}
				}
				return found ? firstindex : npos;
			}
	};

	static inline String operator +(const char* lhs, const String& rhs) { return String(lhs) + String(rhs); }
}
