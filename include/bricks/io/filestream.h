#pragma once

#include "bricks/io/stream.h"
#include "bricks/io/filesystem.h"

namespace Bricks { namespace IO {
	class FileStream : public Stream
	{
	protected:
		AutoPointer<Filesystem> system;
		FileHandle handle;
		String path;

	public:
		FileStream(
			const String& path,
			FileOpenMode::Enum createmode = FileOpenMode::Open,
			FileMode::Enum mode = FileMode::ReadWrite,
			FilePermissions::Enum permissions = FilePermissions::OwnerReadWrite,
			const Pointer<Filesystem>& filesystem = NULL
		) : system(filesystem ? filesystem : Filesystem::GetDefault()), handle(system->Open(path, createmode, mode)), path(path) { }
		FileStream(FileHandle handle, const Pointer<Filesystem>& filesystem = NULL) : system(filesystem ? filesystem : Pointer<Filesystem>(Filesystem::GetDefault())), handle(system->Duplicate(handle)) { }
		~FileStream() { system->Close(handle); }
		size_t Read(void* buffer, size_t size) { return system->Read(handle, buffer, size); }
		size_t Write(const void* buffer, size_t size) { return system->Write(handle, buffer, size); }
		u64 GetLength() const { return system->FileStat(handle).GetSize(); }
		void SetLength(u64 length) { system->Truncate(handle, length); }
		u64 GetPosition() const { return system->Tell(handle); }
		void SetPosition(u64 position) { Seek(position, SeekType::Beginning); }
		void Seek(s64 offset, SeekType::Enum whence) { system->Seek(handle, offset, whence); }
		void Flush() { system->Flush(handle); }

		ReturnPointer<Filesystem> GetFilesystem() const { return system; }
		FileHandle GetHandle() const { return handle; }

		const String& GetPath() const { return path; }
	};
} }
