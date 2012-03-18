#pragma once

#include "bricks/core/autopointer.h"
#include "bricks/io/stream.h"

namespace Bricks { namespace IO {
	class Substream : public Stream
	{
	private:
		AutoPointer<Stream> stream;
		u64 offset;
		u64 position;
		u64 length;

	public:
		Substream(Stream* stream, u64 offset);
		Substream(Stream* stream, u64 offset, u64 length);

		u64 GetOffset() { return offset; }
		Stream* GetStream() { return stream; }

		u64 GetLength() const { return length; }
		void SetLength(u64 value) { length = value; }
		u64 GetPosition() const { return position; }
		void SetPosition(u64 value) { position = value; }

		u64 GetStreamOffset(Stream* parent = NULL);

		size_t Read(void* buffer, size_t size);
		size_t Write(const void* buffer, size_t size);
		void Flush();
	};
} }
