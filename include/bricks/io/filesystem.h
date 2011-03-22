#pragma once

#include "bricks/types.h"

#include "bricks/string.h"

#include <stdio.h>

namespace Bricks { namespace IO {
	typedef size_t FileHandle;

	namespace FileMode { enum Enum {
		ReadOnly = 1,
		WriteOnly = 2,
		ReadWrite = ReadOnly | WriteOnly
	}; }

	namespace FileCreateMode { enum Enum {
		Open,
		Create,
		CreateNew,
		Append,
		Truncate
	}; }

	namespace SeekType { enum Enum {
		Beginning = SEEK_SET,
		Current = SEEK_CUR,
		End = SEEK_END
	}; }

	class Filesystem
	{
	public:
		static Filesystem* GetDefault();

		virtual FileHandle Open(
			const String& path,
			FileCreateMode::Enum createmode = FileCreateMode::Create,
			FileMode::Enum mode = FileMode::ReadWrite
		) = 0;
		virtual size_t Read(
			FileHandle fd,
			void* buffer,
			size_t size
		) = 0;
		virtual size_t Write(
			FileHandle fd,
			const void* buffer,
			size_t size
		) = 0;
		virtual u64 Tell(FileHandle fd) = 0;
		virtual void Seek(FileHandle fd, s64 offset, SeekType::Enum whence) = 0;
		virtual void Flush(FileHandle fd) = 0;
		virtual void Truncate(FileHandle fd, u64 length) = 0;
		virtual void Close(FileHandle fd) = 0;

		//FileInfo Stat(FilePath path);
		//diriter stuff
	};

	class C89Filesystem : public Filesystem
	{
	public:
		FileHandle Open(
			const String& path,
			FileCreateMode::Enum createmode,
			FileMode::Enum mode
		);
		size_t Read(
			FileHandle fd,
			void* buffer,
			size_t size
		);
		size_t Write(
			FileHandle fd,
			const void* buffer,
			size_t size
		);
		u64 Tell(FileHandle fd);
		void Seek(FileHandle fd, s64 offset, SeekType::Enum whence);
		void Flush(FileHandle fd);
		void Close(FileHandle fd);
		void Truncate(FileHandle fd, u64 length);
	};
	
	class PosixFilesystem : public Filesystem
	{
	public:
		FileHandle Open(
			const String& path,
			FileCreateMode::Enum createmode,
			FileMode::Enum mode
		);
		size_t Read(
			FileHandle fd,
			void* buffer,
			size_t size
		);
		size_t Write(
			FileHandle fd,
			const void* buffer,
			size_t size
		);
		u64 Tell(FileHandle fd);
		void Seek(FileHandle fd, s64 offset, SeekType::Enum whence);
		void Flush(FileHandle fd);
		void Close(FileHandle fd);
		void Truncate(FileHandle fd, u64 length);
	};
} }
