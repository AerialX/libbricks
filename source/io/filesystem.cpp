#include "bricks/io/filesystem.h"
#include "bricks/exception.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

namespace Bricks { namespace IO {
	static Filesystem* defaultFilesystem = NULL;
	Filesystem* Filesystem::GetDefault()
	{
		if (!defaultFilesystem)
			//defaultFilesystem = new C89Filesystem();
			defaultFilesystem = new PosixFilesystem();
		return defaultFilesystem;
	}

	FileHandle C89Filesystem::Open(
			const String& path,
			FileCreateMode::Enum createmode,
			FileMode::Enum mode)
	{
		const char* cmode = "rb";
		switch (createmode) {
			case FileCreateMode::Open:
				if (mode == FileMode::ReadOnly)
					cmode = "rb";
				else
					cmode = "r+b";
				break;
			case FileCreateMode::Create:
				if (mode == FileMode::ReadOnly)
					throw InvalidArgumentException("mode");
				else
					cmode = "w+b";
				break;
			case FileCreateMode::CreateNew:
				if (mode == FileMode::ReadOnly)
					throw InvalidArgumentException("mode");
				else
					cmode = "w+b";
				break;
			case FileCreateMode::Append:
				if (mode == FileMode::ReadOnly)
					throw InvalidArgumentException("mode");
				else
					cmode = "a+b";
				break;
			case FileCreateMode::Truncate:
				if (mode == FileMode::ReadOnly)
					throw InvalidArgumentException("mode");
				else
					cmode = "w+b";
				break;
		}
		FILE* ret = fopen(
				path.CString(),
				cmode
		);
		if (!ret)
			ThrowErrno();
		return (FileHandle)ret;
	}

	size_t C89Filesystem::Read(
			FileHandle fd,
			void* buffer,
			size_t size)
	{
		size_t ret = fread(buffer, 1, size, (FILE*)fd);
		if (ret < size)
			ThrowErrno();
		return ret;
	}
	
	size_t C89Filesystem::Write(
			FileHandle fd,
			const void* buffer,
			size_t size)
	{
		size_t ret = fwrite(buffer, 1, size, (FILE*)fd);
		if (ret < size)
			ThrowErrno();
		return ret;
	}
	
	void C89Filesystem::Flush(FileHandle fd)
	{
		throw NotImplementedException();
		// TODO: fsync FILE*?
	}
	
	void C89Filesystem::Seek(FileHandle fd, s64 offset, SeekType::Enum whence)
	{
		if (fseek((FILE*)fd, offset, (int)whence))
			ThrowErrno();
	}

	u64 C89Filesystem::Tell(FileHandle fd)
	{
		long ret = ftell((FILE*)fd);
		if (ret < 0)
			ThrowErrno();
		return (u64)ret;
	}
	
	void C89Filesystem::Close(FileHandle fd)
	{
		if (fclose((FILE*)fd))
			ThrowErrno();
	}
	
	void C89Filesystem::Truncate(FileHandle fd, u64 length)
	{
		throw NotImplementedException();
	}
	
	FileHandle PosixFilesystem::Open(
			const String& path,
			FileCreateMode::Enum createmode,
			FileMode::Enum mode)
	{
		int oflag = 0;
		switch (createmode) {
			case FileCreateMode::Open:
				break;
			case FileCreateMode::Create:
				oflag = O_CREAT;
				break;
			case FileCreateMode::CreateNew:
				oflag = O_CREAT | O_EXCL;
				break;
			case FileCreateMode::Append:
				oflag = O_APPEND;
				break;
			case FileCreateMode::Truncate:
				oflag = O_TRUNC;
				break;
		}
		switch (mode) {
			case FileMode::ReadOnly:
				oflag |= O_RDONLY;
				break;
			case FileMode::WriteOnly:
				oflag |= O_WRONLY;
				break;
			case FileMode::ReadWrite:
				oflag |= O_RDWR;
				break;
		}
		int ret = open(path.CString(), oflag);
		if (ret < 0)
			ThrowErrno();
		return (FileHandle)ret;
	}

	size_t PosixFilesystem::Read(
			FileHandle fd,
			void* buffer,
			size_t size)
	{
		ssize_t ret = read((int)fd, buffer, size);
		if (ret < 0)
			ThrowErrno();
		return ret;
	}
	
	size_t PosixFilesystem::Write(
			FileHandle fd,
			const void* buffer,
			size_t size)
	{
		ssize_t ret = write((int)fd, buffer, size);
		if (ret < 0)
			ThrowErrno();
		return ret;
	}
	
	void PosixFilesystem::Seek(FileHandle fd, s64 offset, SeekType::Enum whence)
	{
		if (lseek64((int)fd, offset, (int)whence) == (off64_t)-1)
			ThrowErrno();
	}

	u64 PosixFilesystem::Tell(FileHandle fd)
	{
		off64_t ret = lseek64((int)fd, 0, SEEK_SET);
		if (ret == (off64_t)-1)
			ThrowErrno();
		return (u64)ret;
	}
	
	void PosixFilesystem::Flush(FileHandle fd)
	{
		if (fsync((int)fd))
			ThrowErrno();
	}
	
	void PosixFilesystem::Close(FileHandle fd)
	{
		if (close((int)fd))
			ThrowErrno();
	}
	
	void PosixFilesystem::Truncate(FileHandle fd, u64 length)
	{
		if (ftruncate64((int)fd, length))
			ThrowErrno();
	}
} }
