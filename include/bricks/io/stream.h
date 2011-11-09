#pragma once

#include "bricks/object.h"

namespace Bricks { namespace IO {
	class Stream : public Object
	{
	public:
		virtual size_t Read(void* buffer, size_t size) = 0;
		virtual size_t Write(const void* buffer, size_t size) = 0;
		virtual u64 GetLength() const = 0;
		virtual void SetLength(u64 length) = 0;
		virtual u64 GetPosition() const = 0;
		virtual void SetPosition(u64 position) = 0;
		virtual void Flush() { }
		virtual int ReadByte() { u8 data; if (Read(&data, sizeof(data)) != sizeof(data)) return -1; return data; }
		virtual bool CanSeek() const { return true; }
		virtual bool CanRead() const { return true; }
		virtual bool CanWrite() const { return true; }
		virtual size_t Read(Data& data) { return Read(data.GetData(), data.GetSize()); }
		virtual size_t Write(const Data& data) { return Write(data.GetData(), data.GetSize()); }
	};
} }
