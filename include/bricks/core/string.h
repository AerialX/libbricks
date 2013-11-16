#pragma once

#include "bricks/core/object.h"
#include "bricks/core/autopointer.h"
#include "bricks/core/data.h"

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
		typedef u32 Character;

		static String FormatVariadic(const String& format, va_list args);
		static String Format(const String& format, ...);

		int ScanVariadic(const String& format, va_list args);
		int Scan(const String& format, ...);

	protected:
		AutoPointer<Data> buffer;
		mutable size_t dataLength;

		void Construct(size_t len);
		void Construct(const char* string, size_t len);

		size_t ConvertStrChr(const char* ptr) const;

	public:
		String();
		String(const String& string);
		String(const String& string, size_t off, size_t len = npos);
		String(const char* string, size_t len = npos);
		String(char character, size_t repeat = 1);
		String(Character character, size_t repeat = 1);
#if BRICKS_ENV_OBJC
		String(NSString* string, size_t len = npos) { Construct([string UTF8String], len); }
#endif

		~String();

		String GetDebugString() const;

		String& operator=(const String& string);
		String& operator=(const char* string);
		String operator+(const String& string) const;
		String& operator+=(const String& string);

		const char* CString() const { return (const char*)buffer->GetData(); }
		Data* GetBuffer() const { return buffer; }
		size_t GetSize() const { return buffer->GetSize() - 1; }

		Character GetCharacter(size_t index) const;
		void SetCharacter(size_t index, Character character);
		Character operator[](size_t index) const { return GetCharacter(index); }

		size_t GetLength() const;

		int Compare(const String& string) const;
		int Compare(const String& string, size_t len) const;
		int Compare(size_t off1, const String& string, size_t off2, size_t len) const;
		int Compare(const char* string) const;
		int Compare(const char* string, size_t len) const;
		int Compare(size_t off1, const char* string) const;
		int Compare(size_t off1, const char* string, size_t len) const;

		bool operator==(const String& rhs) const;
		bool operator!=(const String& rhs) const { return !operator==(rhs); }
		bool operator>(const String& rhs) const { return Compare(rhs) > 0; }
		bool operator<(const String& rhs) const { return Compare(rhs) < 0; }
		bool operator>=(const String& rhs) const { return Compare(rhs) >= 0; }
		bool operator<=(const String& rhs) const { return Compare(rhs) <= 0; }

		void Truncate(size_t len);
		void TruncateSize(size_t len);

		String Substring(size_t off = 0, size_t len = npos) const;

		size_t FirstIndexOf(Character chr, size_t off = 0) const;
		size_t LastIndexOf(Character chr, size_t off = 0) const;

		size_t FirstIndexOf(const String& string, size_t off = 0) const;
		size_t LastIndexOf(const String& string, size_t off = 0) const;
	};

	static inline String operator +(const char* lhs, const String& rhs) { return String(lhs) + rhs; }
	static inline bool operator==(const String& lhs, const char* rhs) { return !lhs.Compare(rhs); }
	static inline bool operator!=(const String& lhs, const char* rhs) { return lhs.Compare(rhs); }
	static inline bool operator==(const char* lhs, const String& rhs) { return !rhs.Compare(lhs); }
	static inline bool operator!=(const char* lhs, const String& rhs) { return rhs.Compare(lhs); }
}
