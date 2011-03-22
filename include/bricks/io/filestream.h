#pragma once

#include "bricks/io/filesystem.h"

#include "bricks/io/stream.h"

namespace Bricks { namespace IO {
	class FileStream : public Stream
	{
	protected:
		Filesystem* System;
		FileHandle Handle;

	public:
		FileStream(
			const String& path,
			FileCreateMode::Enum createmode = FileCreateMode::Create,
			FileMode::Enum mode = FileMode::ReadWrite,
			Filesystem* filesystem = NULL
		) { System = filesystem ?: Filesystem::GetDefault(); System->Open(path, createmode, mode); }
		size_t Read(void* buffer, size_t size) { return System->Read(Handle, buffer, size); }
		size_t Write(const void* buffer, size_t size) { return System->Write(Handle, buffer, size); }
		u64 GetLength();
		void SetLength(u64 length) { System->Truncate(Handle, length); }
		u64 GetPosition() { return System->Tell(Handle); }
		void SetPosition(u64 position) { Seek(position, SeekType::Beginning); }
		void Seek(s64 offset, SeekType::Enum whence) { System->Seek(Handle, offset, whence); }
		void Flush() { System->Flush(Handle); }
		void Close() { System->Close(Handle); }
	};
} }
