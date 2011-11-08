#pragma once

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
		Substream(const Pointer<Stream>& stream, u64 offset);
		Substream(const Pointer<Stream>& stream, u64 offset, u64 length);

		u64 GetOffset() { return offset; }
		ReturnPointer<Stream> GetStream() { return stream; }

		u64 GetLength() const { return length; }
		void SetLength(u64 value) { length = value; }
		u64 GetPosition() const { return position; }
		void SetPosition(u64 value) { position = value; }

		u64 GetStreamOffset(const Pointer<Stream>& parent = NULL);

		size_t Read(void* buffer, size_t size);
		size_t Write(const void* buffer, size_t size);
		void Flush();
	};
} }
