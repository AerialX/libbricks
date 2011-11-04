#pragma once

#include "bricks/io/stream.h"

namespace Bricks { namespace IO {
	class NullStream : public Stream
	{
	public:
		size_t Read(void* buffer, size_t size) { return 0; }
		size_t Write(const void* buffer, size_t size) { return 0; }
		u64 GetLength() const { return 0; }
		void SetLength(u64 length) { }
		u64 GetPosition() const { return 0; }
		void SetPosition(u64 position) { }
	};
} }
