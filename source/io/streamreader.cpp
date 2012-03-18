#include "bricks/io/streamreader.h"
#include "bricks/io/stream.h"
#include "bricks/core/data.h"
#include "bricks/core/math.h"

namespace Bricks { namespace IO {
	StreamReader::StreamReader(Stream* stream, Endian::Enum endianness) :
		StreamNavigator(stream, endianness)
	{

	}

#define BRICKS_STREAM_READ(size) \
	u##size StreamReader::ReadInt##size(Endian::Enum endian) \
	{ \
		char data[sizeof(u##size)]; \
		if (stream->Read(data, sizeof(u##size)) != sizeof(u##size)) \
			BRICKS_FEATURE_THROW(EndOfStreamException()); \
		return EndianConvert##size<u##size>(endian ?: endianness, data); \
	}
	BRICKS_STREAM_READ(16);
	BRICKS_STREAM_READ(32);
	BRICKS_STREAM_READ(64);
#undef BRICKS_STREAM_READ

	u8 StreamReader::ReadByte()
	{
		u8 data;
		if (stream->Read(&data, sizeof(data)) != sizeof(data))
			BRICKS_FEATURE_THROW(EndOfStreamException());
		return data;
	}

	void StreamReader::ReadBytes(void* data, size_t size)
	{
		if (stream->Read(data, size) != size)
			BRICKS_FEATURE_THROW(EndOfStreamException());
	}

	Data StreamReader::ReadBytes(size_t size)
	{
		Data data(size);
		if (stream->Read(data, size) != size)
			BRICKS_FEATURE_THROW(EndOfStreamException());
		return data;
	}

	String StreamReader::ReadCString(int division)
	{
		// TODO: StringBuilder, this is fail.
		String ret;
		while (true) {
			int read = stream->ReadByte();
			if (read < 0 || read == division)
				break;
			ret += (char)read;
		}
		return ret;
	}

	String StreamReader::ReadString(int length)
	{
		char buffer[length];
		ReadBytes(buffer, length);
		return String(buffer, length);
	}

	String StreamReader::ReadString()
	{
		return ReadCString('\0');
	}

	void StreamReader::Pad(u64 size)
	{
		u8 padding[0x100];
		while (size > 0) {
			size_t sz = Math::Min(sizeof(padding), size);
			if (stream->Read(padding, sz) != sz)
				BRICKS_FEATURE_THROW(EndOfStreamException());
			size -= sz;
		}
	}
} }
