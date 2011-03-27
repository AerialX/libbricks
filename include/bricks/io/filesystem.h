#pragma once

#include "bricks.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

namespace Bricks { namespace IO {
	typedef size_t FileHandle;

	namespace FileMode { enum Enum {
		ReadOnly = O_RDONLY,
		WriteOnly = O_WRONLY,
		ReadWrite = O_RDWR
	}; }

	namespace FileOpenMode { enum Enum {
		Open		= 0,
		Create		= O_CREAT | O_TRUNC,
		CreateNew	= O_CREAT | O_EXCL,
		Append		= O_APPEND,
		Truncate	= O_TRUNC
	}; }

	namespace FilePermissions { enum Enum {
		OwnerAll		= S_IRWXU,
		OwnerRead		= S_IRUSR,
		OwnerWrite		= S_IWUSR,
		OwnerReadWrite	= OwnerRead | OwnerWrite,
		OwnerExecute	= S_IXUSR,
		GroupAll		= S_IRWXG,
		GroupRead		= S_IRGRP,
		GroupWrite		= S_IWGRP,
		GroupReadWrite	= GroupRead | GroupWrite,
		GroupExecute	= S_IXGRP,
		OtherAll		= S_IRWXO,
		OtherRead		= S_IROTH,
		OtherWrite		= S_IWOTH,
		OtherReadWrite	= OtherRead | OtherWrite,
		OtherExecute	= S_IXOTH,
		SetUserID		= S_ISUID,
		SetGroupID		= S_ISGID,
		Sticky			= S_ISVTX
	}; }

	namespace SeekType { enum Enum {
		Beginning = SEEK_SET,
		Current = SEEK_CUR,
		End = SEEK_END
	}; }

	class Filesystem : public Object
	{
	public:
		static Filesystem& GetDefault();

		virtual FileHandle Open(
			const String& path,
			FileOpenMode::Enum createmode = FileOpenMode::Create,
			FileMode::Enum mode = FileMode::ReadWrite,
			FilePermissions::Enum permissions = FilePermissions::OwnerReadWrite
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
		virtual u64 Tell(FileHandle fd) const = 0;
		virtual void Seek(FileHandle fd, s64 offset, SeekType::Enum whence) = 0;
		virtual void Flush(FileHandle fd) = 0;
		virtual void Truncate(FileHandle fd, u64 length) = 0;
		virtual void Close(FileHandle fd) = 0;

		//FileInfo Stat(const String& path) const = 0;
		//FileInfo Stat(FileHandle fd) const = 0;
		//diriter stuff
	};

	class C89Filesystem : public Filesystem
	{
	public:
		FileHandle Open(
			const String& path,
			FileOpenMode::Enum createmode,
			FileMode::Enum mode,
			FilePermissions::Enum permissions
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
		u64 Tell(FileHandle fd) const;
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
			FileOpenMode::Enum createmode,
			FileMode::Enum mode,
			FilePermissions::Enum permissions
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
		u64 Tell(FileHandle fd) const;
		void Seek(FileHandle fd, s64 offset, SeekType::Enum whence);
		void Flush(FileHandle fd);
		void Close(FileHandle fd);
		void Truncate(FileHandle fd, u64 length);
	};
} }
