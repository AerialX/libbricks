#include "bricksall.hpp"

namespace Bricks { namespace IO {
	CacheStream::CacheStream(const Pointer<Stream>& stream, u32 cacheSize) :
		stream(stream), cacheSize(cacheSize),
		cacheDirty(true), cacheOffset(0), cacheLength(0), cachePosition(0)
	{
		cache = new u8[cacheSize];
		length = stream->GetLength();
		position = stream->GetPosition();
	}

	CacheStream::~CacheStream()
	{
		delete[] cache;
	}

	void CacheStream::FillCache()
	{
		if (cacheDirty)
			FlushCache();

		if (position != cachePosition + cacheOffset)
			stream->SetPosition(position);
		cachePosition = position;
		cacheLength = stream->Read(cache, cacheSize);
		cacheOffset = 0;
	}

	void CacheStream::FlushCache()
	{
		if (cacheDirty) {
			if (cacheOffset && stream->Write(cache, cacheOffset) != cacheOffset)
				throw Exception();
		}

		cachePosition += cacheLength;

		cacheDirty = false;
		cacheOffset = 0;
		cacheLength = 0;
	}

	void CacheStream::DirtyCache()
	{
		cacheDirty = false;
		cachePosition = position;
		cacheOffset = 0;
		cacheLength = 0;
	}

	void CacheStream::Flush()
	{
		FlushCache();
		stream->Flush();
	}

	size_t CacheStream::Read(void* buffer, size_t size)
	{
		size_t length = 0;

		if (cacheDirty)
			FlushCache();

		if (position >= cachePosition && position < cachePosition + cacheLength) {
			length = BRICKS_FEATURE_MIN(size, cacheLength - cacheOffset);
			if (length) {
				memcpy(buffer, cache + cacheOffset, length);
				cacheOffset += length;
				position += length;
			}

			if (length >= size)
				return length;
		}

		FillCache();
		return length + Read((u8*)buffer + length, size - length);
	}

	size_t CacheStream::Write(const void* buffer, size_t size)
	{
		if (!cacheDirty)
			DirtyCache();

		size_t length = BRICKS_FEATURE_MIN(size, cacheSize - cacheOffset);
		if (length) {
			memcpy(cache + cacheOffset, buffer, length);
			cacheOffset += length;
			position += length;
			cacheLength = cacheOffset;
			cacheDirty = true;
		}

		if (length >= size)
			return size;

		FlushCache();
		return length + Write((u8*)buffer + length, size - length);
	}
} }
