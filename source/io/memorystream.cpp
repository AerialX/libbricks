#include "bricks/io/memorystream.h"
#include "bricks/core/math.h"

#include <stdlib.h>
#include <string.h>

namespace Bricks { namespace IO {
	MemoryStream::MemoryStream() :
		data(NULL), length(0), position(0), allocated(0)
	{

	}

	MemoryStream::MemoryStream(unsigned long size) :
		data(NULL), length(0), position(0), allocated(0)
	{
		Allocate(size);
	}

	MemoryStream::MemoryStream(const MemoryStream& stream) :
		data(NULL), length(stream.length), allocated(0)
	{
		Allocate(length);
		memcpy(data, stream.data, length);
	}

	MemoryStream::MemoryStream(const Data& data) :
		data(NULL), length(data.GetLength()), allocated(0)
	{
		Allocate(length);
		memcpy(this->data, data.GetData(), length);
	}

	MemoryStream::MemoryStream(const void* data, unsigned long length) :
		data(NULL), length(length), allocated(0)
	{
		Allocate(length);
		memcpy(this->data, data, length);
	}

	MemoryStream::~MemoryStream()
	{
		free(data);
	}

	MemoryStream& MemoryStream::operator =(const MemoryStream& stream)
	{
		length = stream.length;
		Allocate(length);
		memcpy(data, stream.data, length);
		return *this;
	}

	void MemoryStream::Allocate(unsigned long size)
	{
		allocated = Math::RoundUp(size, BlockSize);
		data = (u8*)realloc(data, allocated);
	}

	size_t MemoryStream::Read(void* buffer, size_t size)
	{
		size = Math::Min(size, length - position);
		memcpy(buffer, data + position, size);
		position += size;
		return size;
	}

	size_t MemoryStream::Write(const void* buffer, size_t size)
	{
		if (position + size > length)
			SetLength(position + size);
		memcpy(data + position, buffer, size);
		position += size;
		return size;
	}

	void MemoryStream::SetLength(u64 length)
	{
		this->length = length;
		Allocate(length);
	}

	void MemoryStream::SetPosition(u64 position)
	{
		this->position = position;
	}

	int MemoryStream::ReadByte()
	{
		return data[position++];
	}
} }
