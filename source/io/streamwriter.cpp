#include "bricks/io/streamwriter.h"
#include "bricks/io/stream.h"
#include "bricks/core/math.h"

namespace Bricks { namespace IO {
	StreamWriter::StreamWriter(Stream* stream, Endian::Enum endianness) :
		StreamNavigator(stream, endianness)
	{

	}

#define BRICKS_STREAM_WRITE(size) \
	void StreamWriter::WriteInt(u##size value, Endian::Enum endian) \
	{ \
		u##size data; \
		EndianConvert##size(endian ?: endianness, &data, value); \
		if (stream->Write(&data, sizeof(u##size)) != sizeof(u##size)) \
		BRICKS_FEATURE_THROW(StreamException()); \
	}
	BRICKS_STREAM_WRITE(16);
	BRICKS_STREAM_WRITE(32);
	BRICKS_STREAM_WRITE(64);
#undef BRICKS_STREAM_WRITE

	void StreamWriter::WriteByte(u8 data)
	{
		if (stream->Write(&data, sizeof(data)) != sizeof(data))
			BRICKS_FEATURE_THROW(StreamException());
	}

	void StreamWriter::WriteBytes(const void* data, size_t size)
	{
		if (stream->Write(data, size) != size)
			BRICKS_FEATURE_THROW(StreamException());
	}

	void StreamWriter::WriteString(const String& str, size_t size)
	{
		if (size == String::npos)
			size = str.GetSize();
		WriteBytes(str.CString(), Math::Min(str.GetSize(), size));
		if (size > str.GetSize())
			Pad(size - str.GetSize());
	}

	void StreamWriter::Pad(u64 size)
	{
		u8 padding[0x100];
		while (size > 0) {
			size_t sz = Math::Min(sizeof(padding), size);
			if (stream->Write(padding, sz) != sz)
				BRICKS_FEATURE_THROW(StreamException());
			size -= sz;
		}
	}

	void StreamWriter::WriteLine(const String& string)
	{
		// TODO: Implement a proper TextWriter with encoding support.
		WriteString(string);
		WriteByte('\n');
	}
} }
