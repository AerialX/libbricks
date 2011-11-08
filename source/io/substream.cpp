#include "bricksall.hpp"

namespace Bricks { namespace IO {
	Substream::Substream(const Pointer<Stream>& stream, u64 offset) :
		stream(stream), offset(offset), position(0)
	{
		length = stream->GetLength() - offset;
	}

	Substream::Substream(const Pointer<Stream>& stream, u64 offset, u64 length) :
		stream(stream), offset(offset), position(0), length(length)
	{

	}

	void Substream::Flush()
	{
		stream->Flush();
	}

	size_t Substream::Read(void* buffer, size_t size)
	{
		if (position + size > length)
			size = length - position;
		if (stream->GetPosition() != offset + position)
			stream->SetPosition(offset + position);
		size = stream->Read(buffer, size);
		position += size;
		return size;
	}

	size_t Substream::Write(const void* buffer, size_t size)
	{
		if (position + size > length)
			size = length - position;
		if (stream->GetPosition() != offset + position)
			stream->SetPosition(offset + position);
		size = stream->Write(buffer, size);
		position += size;
		return size;
	}

	u64 Substream::GetStreamOffset(const Pointer<Stream>& parent)
	{
		Pointer<Substream> substream = stream.AsType<Substream>();
		if (substream && stream != parent)
			return substream->GetStreamOffset(parent) + offset;
		return offset;
	}
} }
