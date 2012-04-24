#include "bricks/core/string.h"
#include "bricks/core/exception.h"

#include <stdarg.h>
#include <stdio.h>

namespace Bricks {
	const String String::Empty;

	static size_t UTF8GetCharacterSize(String::Character character)
	{
		if (!character)
			return 2;

		if (character <= 0x7F)
			return 1;
		else if (character <= 0x7FF)
			return 2;
		else if (character <= 0xFFFF)
			return 3;
		return 4;
	}

	static int UTF8EncodeCharacter(u8* data, String::Character character)
	{
#define UTF8_ENCODE_CHAR(length) UTF8_ENCODE_CHAR_##length(length)
#define UTF8_ENCODE_CHAR_HEADER_0 0x00
#define UTF8_ENCODE_CHAR_HEADER_2 0xC0
#define UTF8_ENCODE_CHAR_HEADER_3 0xE0
#define UTF8_ENCODE_CHAR_HEADER_4 0xF0
#define UTF8_ENCODE_CHAR_MASK_0 0x7F
#define UTF8_ENCODE_CHAR_MASK_2 0x1F
#define UTF8_ENCODE_CHAR_MASK_3 0x0F
#define UTF8_ENCODE_CHAR_MASK_4 0x07
#define UTF8_ENCODE_CHAR_0(length) data[0] = UTF8_ENCODE_CHAR_HEADER_##length | (character & UTF8_ENCODE_CHAR_MASK_##length)
#define UTF8_ENCODE_CHAR_1(index) data[index] = (character & 0x3F); character >>= 6
#define UTF8_ENCODE_CHAR_2(length) UTF8_ENCODE_CHAR_1(1); UTF8_ENCODE_CHAR_0(length)
#define UTF8_ENCODE_CHAR_3(length) UTF8_ENCODE_CHAR_1(2); UTF8_ENCODE_CHAR_2(length)
#define UTF8_ENCODE_CHAR_4(length) UTF8_ENCODE_CHAR_1(3); UTF8_ENCODE_CHAR_3(length)
		switch (UTF8GetCharacterSize(character)) {
			case 1:
				UTF8_ENCODE_CHAR(0);
				return 1;
			case 2:
				UTF8_ENCODE_CHAR(2);
				return 2;
			case 3:
				UTF8_ENCODE_CHAR(3);
				return 3;
			case 4:
				UTF8_ENCODE_CHAR(4);
				return 4;
			default:
				return 0;
		}
	}

	static size_t UTF8GetCharacterSize(const u8* data)
	{
		if (!(*data & 0x80))
			return 1;

		switch (*data & 0xF0) {
			case UTF8_ENCODE_CHAR_HEADER_4:
				return 4;
			case UTF8_ENCODE_CHAR_HEADER_3:
				return 3;
			default:
				return 2;
		}
	}

	static String::Character UTF8DecodeCharacter(const u8* data)
	{
#define UTF8_DECODE_CHAR(length) UTF8_DECODE_CHAR_##length(length)
#define UTF8_DECODE_CHAR_HEADER_1 0x7F
#define UTF8_DECODE_CHAR_HEADER_2 0x1F
#define UTF8_DECODE_CHAR_HEADER_3 0x0F
#define UTF8_DECODE_CHAR_HEADER_4 0x07
#define UTF8_DECODE_CHAR_0(length) (((String::Character)data[0] & UTF8_DECODE_CHAR_HEADER_##length) << ((length - 1) * 6))
#define UTF8_DECODE_CHAR_1(length, index) (((String::Character)data[index] & 0x3F) << ((length - index - 1) * 6))
#define UTF8_DECODE_CHAR_2(length) (UTF8_DECODE_CHAR_0(length) | UTF8_DECODE_CHAR_1(length, 1))
#define UTF8_DECODE_CHAR_3(length) (UTF8_DECODE_CHAR_2(length) | UTF8_DECODE_CHAR_1(length, 2))
#define UTF8_DECODE_CHAR_4(length) (UTF8_DECODE_CHAR_3(length) | UTF8_DECODE_CHAR_1(length, 3))
		switch (UTF8GetCharacterSize(data)) {
			case 4:
				return UTF8_DECODE_CHAR(4);
			case 3:
				return UTF8_DECODE_CHAR(3);
			case 2:
				return UTF8_DECODE_CHAR(2);
			default:
				return UTF8_DECODE_CHAR_0(1);
		}
	}

	static size_t UTF8GetStringLength(const char* data)
	{
		size_t count = 0;
		while (*data) {
			if (!(*data & 0x80)) {
				do {
					count++;
					data++;
				} while (*data && !(*data & 0x80));
				if (!*data)
					break;
			}
			switch (*data & 0xF0) {
				case UTF8_ENCODE_CHAR_HEADER_4:
					data++;
				case UTF8_ENCODE_CHAR_HEADER_3:
					data++;
				default:
					data += 2;
					count++;
					break;
			}
		}
		return count;
	}

	static size_t UTF8GetStringOffset(const char* data, int index)
	{
		const char* origData = data;
		while (index > 0 && *data) {
			if (!(*data & 0x80)) {
				do {
					index--;
					data++;
				} while (index > 0 && *data && !(*data & 0x80));
				if (!index || !*data)
					break;
			}
			switch (*data & 0xF0) {
				case UTF8_ENCODE_CHAR_HEADER_4:
					data++;
				case UTF8_ENCODE_CHAR_HEADER_3:
					data++;
				default:
					data += 2;
					index--;
					break;
			}
		}
		return data - origData;
	}

	void String::Construct(size_t len)
	{
		buffer = autonew Data(len + 1);
		buffer->SetValue(len, 0);
		dataLength = 0;
	}

	void String::Construct(const char* string, size_t len)
	{
		if (len == npos)
			len = string ? strlen(string) : 0;
		Construct(len);
		strncpy((char*)buffer->GetData(), string, len);
	}

	String::String() :
		dataLength(0)
	{
		Construct(0);
	}

	String::String(const String& string) :
		buffer(string.GetBuffer()),
		dataLength(string.dataLength)
	{

	}

	String::String(const String& string, size_t off, size_t len) :
		dataLength(0)
	{
		size_t offset = UTF8GetStringOffset(string.CString(), off);
		size_t length = len == npos ? string.GetSize() - offset : UTF8GetStringOffset(string.CString() + offset, len);
		Construct(string.CString() + offset, length);
		dataLength = len == npos ? 0 : len;
	}

	String::String(const char* string, size_t len)
	{
		size_t length = len == npos ? strlen(string) : UTF8GetStringOffset(string, len);
		Construct(string, length);
		dataLength = len == npos ? 0 : len;
	}

	String::String(char character, size_t repeat)
	{
		Construct(repeat);
		memset(buffer->GetData(), character, repeat);
	}

	String::String(String::Character character, size_t repeat)
	{
		size_t size = UTF8GetCharacterSize(character);
		Construct(repeat * size);
		for (size_t i = 0; i < repeat; i++)
			UTF8EncodeCharacter((u8*)buffer->GetData() + i * size, character);
	}

	String::~String()
	{

	}

	String String::GetDebugString() const
	{
		return Format("\"%s\" [%d]", CString(), GetReferenceCount());
	}

	String& String::operator=(const String& string)
	{
		buffer = string.buffer;
		dataLength = string.dataLength;
		return *this;
	}

	String& String::operator=(const char* string)
	{
		Construct(string, npos);
		return *this;
	}

	String String::operator+(const String& string) const
	{
		String out;
		out.Construct(CString(), GetSize() + string.GetSize());
		memcpy((char*)out.GetBuffer()->GetData() + GetSize(), string.GetBuffer()->GetData(), string.GetSize());
		return out;
	}

	String& String::operator+=(const String& string)
	{
		AutoPointer<Data> newbuffer = autonew Data(GetSize() + string.GetSize() + 1);
		memcpy((char*)newbuffer->GetData(), buffer->GetData(), GetSize());
		memcpy((char*)newbuffer->GetData() + GetSize(), string.GetBuffer()->GetData(), string.GetBuffer()->GetSize());
		buffer = newbuffer;
		return *this;
	}

	String::Character String::GetCharacter(size_t index) const
	{
		size_t offset = UTF8GetStringOffset(CString(), index);
		return UTF8DecodeCharacter((const u8*)CString() + offset);
	}

	void String::SetCharacter(size_t index, String::Character character)
	{
		size_t offset = UTF8GetStringOffset(CString(), index);
		size_t sourceSize = UTF8GetCharacterSize((const u8*)buffer->GetData() + offset);
		size_t destSize = UTF8GetCharacterSize(character);
		if (sourceSize > destSize) {
			memmove((u8*)buffer->GetData() + offset + destSize, (const u8*)buffer->GetData() + offset + sourceSize, buffer->GetSize() - offset - sourceSize);
			TruncateSize(GetSize() - (sourceSize - destSize));
		} else if (destSize > sourceSize) {
			AutoPointer<Data> newbuffer = autonew Data(buffer->GetSize() + destSize - sourceSize);
			memcpy(newbuffer->GetData(), buffer->GetData(), offset);
			memcpy((u8*)newbuffer->GetData() + offset + destSize, (const u8*)buffer->GetData() + offset + sourceSize, buffer->GetSize() - offset - sourceSize);
			buffer = newbuffer;
		}
		UTF8EncodeCharacter((u8*)buffer->GetData() + offset, character);
	}

	bool String::operator==(const String& rhs) const
	{
		return GetSize() == rhs.GetSize() && !memcmp(CString(), rhs.CString(), GetSize());
	}

	size_t String::GetLength() const
	{
		return dataLength ?: (dataLength = UTF8GetStringLength(CString()));
	}

	int String::Compare(const String& string) const
	{
		return Compare(0, string.CString());
	}

	int String::Compare(const String& string, size_t len) const
	{
		return Compare(0, string.CString(), len);
	}

	int String::Compare(size_t off1, const String& string, size_t off2, size_t len) const
	{
		return Compare(off1, string.CString() + off2, len);
	}

	int String::Compare(const char* string) const
	{
		return Compare(0, string);
	}

	int String::Compare(const char* string, size_t len) const
	{
		return Compare(0, string, len);
	}

	int String::Compare(size_t off1, const char* string, size_t len) const
	{
		return strncmp(CString() + off1, string, len);
	}

	int String::Compare(size_t off1, const char* string) const
	{
		return strcmp(CString() + off1, string);
	}

	void String::Truncate(size_t len)
	{
		size_t length = UTF8GetStringOffset(CString(), len);
		buffer->SetSize(length + 1);
		buffer->SetValue(length, 0);
	}

	void String::TruncateSize(size_t len)
	{
		buffer->SetSize(len + 1);
		buffer->SetValue(len, 0);
	}

	String String::Substring(size_t off, size_t len) const
	{
		return String(CString() + (off == npos ? 0 : UTF8GetStringOffset(CString(), off)), len);
	}

	size_t String::FirstIndexOf(String::Character chr, size_t off) const
	{
		const u8* charData = (const u8*)CString() + UTF8GetStringOffset(CString(), off);
		size_t index = off;
		while (*charData) {
			if (UTF8DecodeCharacter(charData) == chr)
				return index;
			charData += UTF8GetCharacterSize(charData);
			index++;
		}
		return npos;
	}

	size_t String::LastIndexOf(String::Character chr, size_t off) const
	{
		const u8* charData = (const u8*)CString() + UTF8GetStringOffset(CString(), off);
		size_t index = off;
		size_t lastIndex = npos;
		while (*charData) {
			if (UTF8DecodeCharacter(charData) == chr)
				lastIndex = index;
			charData += UTF8GetCharacterSize(charData);
			index++;
		}
		return lastIndex;
	}

	size_t String::FirstIndexOf(const String& string, size_t off) const
	{
		size_t count = string.GetLength();
		size_t firstindex = GetLength();
		for (size_t i = 0; i < count; i++) {
			size_t index = FirstIndexOf(string[i], off);
			if (index != npos && index < firstindex)
				firstindex = index;
		}
		return firstindex == GetLength() ? npos : firstindex;
	}

	size_t String::LastIndexOf(const String& string, size_t off) const
	{
		size_t count = string.GetLength();
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

	String String::FormatVariadic(const String& format, va_list args)
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
		String ret = FormatVariadic(format, args);
		va_end(args);
		return ret;
	}

	int String::ScanVariadic(const String& format, va_list args)
	{
		return vsscanf(CString(), format.CString(), args);
	}

	int String::Scan(const String& format, ...)
	{
		va_list args;
		va_start(args, format);
		int ret = ScanVariadic(format, args);
		va_end(args);
		return ret;
	}
}
