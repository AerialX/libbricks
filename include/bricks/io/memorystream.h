#pragma once

#include "bricks/core/math.h"
#include "bricks/io/stream.h"

#include <stdlib.h>
#include <string.h>

namespace Bricks { namespace IO {
	class MemoryStream : public Stream
	{
	protected:
		u8* data;
		unsigned long length;
		unsigned long position;
		unsigned long allocated;

		static const int BlockSize = 0x400;

	public:
		MemoryStream() : data(NULL), length(0), position(0), allocated(0) { }
		MemoryStream(unsigned long size) : data(NULL), length(0), position(0), allocated(0) { Allocate(size); }
		MemoryStream(const MemoryStream& stream) : data(NULL), length(stream.length), allocated(0) { Allocate(length); memcpy(data, stream.data, length); }
		MemoryStream(const Data& data) : data(NULL), length(data.GetLength()), allocated(0) { Allocate(length); memcpy(this->data, data.GetData(), length); }
		MemoryStream(const void* data, unsigned long length) : data(NULL), length(length), allocated(0) { Allocate(length); memcpy(this->data, data, length); }
		~MemoryStream() { free(data); }

		MemoryStream& operator =(const MemoryStream& stream) { length = stream.length; Allocate(length); memcpy(data, stream.data, length); return *this; }

		void Allocate(unsigned long size) {
			allocated = Math::RoundUp(size, BlockSize);
			data = (u8*)realloc(data, allocated);
		}

		size_t Read(void* buffer, size_t size) {
			size = Math::Min(size, length - position);
			memcpy(buffer, data + position, size);
			position += size;
			return size;
		}

		size_t Write(const void* buffer, size_t size) {
			if (position + size > length)
				SetLength(position + size);
			memcpy(data + position, buffer, size);
			position += size;
			return size;
		}

		u64 GetLength() const { return length; }
		void SetLength(u64 length) { this->length = length; Allocate(length); }
		u64 GetPosition() const { return position; }
		void SetPosition(u64 position) { this->position = position; }
		int ReadByte() { return data[position++]; }

		void* GetBuffer() { return data; }
	};
} }
