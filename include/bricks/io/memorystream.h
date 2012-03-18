#pragma once

#include "bricks/io/stream.h"

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
		MemoryStream();
		MemoryStream(unsigned long size);
		MemoryStream(const MemoryStream& stream);
		MemoryStream(const Data& data);
		MemoryStream(const void* data, unsigned long length);
		~MemoryStream();

		MemoryStream& operator =(const MemoryStream& stream);

		void Allocate(unsigned long size);

		size_t Read(void* buffer, size_t size);

		size_t Write(const void* buffer, size_t size);

		void SetLength(u64 length);
		void SetPosition(u64 position);
		int ReadByte();

		void* GetBuffer() { return data; }
		u64 GetLength() const { return length; }
		u64 GetPosition() const { return position; }
	};
} }
