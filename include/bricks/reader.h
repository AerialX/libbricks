#pragma once

#include "bricks/io/endian.h"

namespace Bricks { namespace IO {
	class StreamReader
	{
	protected:
		Stream BaseStream;
		Endianness::Enum Endian;

	public:
		StreamReader(Stream stream, Endianness::Enum endianness) : BaseStream(stream), Endian(endianness) { }

	};
} }
