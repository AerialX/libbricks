#pragma once

#include "bricks/io/streamnavigator.h"

namespace Bricks { class Data; }

namespace Bricks { namespace IO {
	class StreamReader : public StreamNavigator
	{
	public:
		StreamReader(Stream* stream, Endian::Enum endianness = Endian::Native);

		u16 ReadInt16(Endian::Enum endian = Endian::Unknown);
		u32 ReadInt32(Endian::Enum endian = Endian::Unknown);
		u64 ReadInt64(Endian::Enum endian = Endian::Unknown);

		u8 ReadByte();
		void ReadBytes(void* data, size_t size);
		Data ReadBytes(size_t size);

		String ReadCString(int division);

		String ReadString(int length);

		String ReadString();

		void Pad(u64 size);
	};
} }
