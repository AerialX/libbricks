#pragma once

#include "bricks/types.h"

namespace Bricks { namespace IO {
	class Stream
	{
	public:
		virtual size_t Read(void* buffer, size_t size) = 0;
		virtual size_t Write(const void* buffer, size_t size) = 0;
		virtual u64 GetLength() = 0;
		virtual void SetLength(u64 length) = 0;
		virtual u64 GetPosition() = 0;
		virtual void SetPosition(u64 position) = 0;
		virtual void Flush() { }
	};
} }
