#pragma once

#include "bricks/io/streamnavigator.h"
#include "bricks/io/endian.h"

namespace Bricks { namespace IO {
	class StreamWriter : public StreamNavigator
	{
	public:
		StreamWriter(Stream* stream, Endian::Enum endianness = Endian::Native);

		void WriteInt(u16 value, Endian::Enum endian = Endian::Unknown);
		void WriteInt(u32 value, Endian::Enum endian = Endian::Unknown);
		void WriteInt(u64 value, Endian::Enum endian = Endian::Unknown);
		void WriteInt16(u16 value, Endian::Enum endian = Endian::Unknown) { WriteInt(value, endian); }
		void WriteInt32(u32 value, Endian::Enum endian = Endian::Unknown) { WriteInt(value, endian); }
		void WriteInt64(u64 value, Endian::Enum endian = Endian::Unknown) { WriteInt(value, endian); }

		void WriteByte(u8 data);
		void WriteBytes(const void* data, size_t size);
		void WriteString(const String& str, size_t size = String::npos);

		void Pad(u64 size);

		void WriteLine(const String& string = String::Empty);
	};
} }
