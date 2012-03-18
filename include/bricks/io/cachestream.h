#pragma once

#include "bricks/core/copypointer.h"
#include "bricks/io/stream.h"

namespace Bricks { namespace IO {
	class CacheStream : public Stream, NoCopy
	{
	private:
		AutoPointer<Stream> stream;
		u32 cacheSize;
		u64 position;
		u64 length;
		u8* cache;
		bool cacheDirty;
		u32 cacheOffset;
		u32 cacheLength;
		u64 cachePosition;

		void FillCache();
		void DirtyCache();
		void FlushCache();

	public:
		CacheStream(Stream* stream, u32 cacheSize = 0x10000);
		~CacheStream();

		u32 GetCacheSize() { return cacheSize; }
		Stream* GetStream() { return stream; }

		u64 GetLength() const { return length; }
		void SetLength(u64 value) { length = value; }
		u64 GetPosition() const { return position; }
		void SetPosition(u64 value) { position = value; }

		size_t Read(void* buffer, size_t size);
		size_t Write(const void* buffer, size_t size);
		void Flush();
	};
} }
