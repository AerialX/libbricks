#include "bricksall.hpp"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>

#ifdef BRICKS_FEATURE_APPLE
#define off64_t off_t
#define lseek64 lseek
#define ftruncate64 ftruncate
#endif
#if defined(BRICKS_FEATURE_MINGW) || defined(BRICKS_FEATURE_ANDROID)
#define ftruncate64 ftruncate
#endif

namespace Bricks { namespace IO {
	const String FilePath::DirectorySeparators = String("/\\");

	static Pointer<Filesystem> defaultFilesystem = NULL;
	const Pointer<Filesystem>& Filesystem::GetDefault()
	{
		if (!defaultFilesystem)
			//defaultFilesystem = GlobalAlloc<C89Filesystem>();
			defaultFilesystem = GlobalAlloc<PosixFilesystem>();
		return defaultFilesystem;
	}

	FileHandle C89Filesystem::Open(
			const String& path,
			FileOpenMode::Enum createmode,
			FileMode::Enum mode,
			FilePermissions::Enum permissions)
	{
		const char* cmode = "rb";
		switch (createmode) {
			case FileOpenMode::Open:
				if (mode == FileMode::ReadOnly)
					cmode = "rb";
				else
					cmode = "r+b";
				break;
			case FileOpenMode::Create:
				if (mode == FileMode::ReadOnly)
					throw InvalidArgumentException("mode");
				else
					cmode = "w+b";
				break;
			case FileOpenMode::CreateNew:
				if (mode == FileMode::ReadOnly)
					throw InvalidArgumentException("mode");
				else
					cmode = "w+b";
				break;
			case FileOpenMode::Append:
				if (mode == FileMode::ReadOnly)
					throw InvalidArgumentException("mode");
				else
					cmode = "a+b";
				break;
			case FileOpenMode::Truncate:
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

	u64 C89Filesystem::Tell(FileHandle fd) const
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
			FileOpenMode::Enum createmode,
			FileMode::Enum mode,
			FilePermissions::Enum permissions)
	{
		int oflag = createmode | mode;
		int ret = open(path.CString(), oflag, permissions); // FIXME: On Windows, oflag | O_BINARY
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

	u64 PosixFilesystem::Tell(FileHandle fd) const
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
	
	FileHandle PosixFilesystem::OpenDirectory(const String& path)
	{
		DIR* dir = opendir(path.CString());
		if (!dir)
			ThrowErrno();
		return (FileHandle)dir;
	}
	
	Pointer<FileNode> PosixFilesystem::ReadDirectory(FileHandle fd)
	{
		errno = 0;
		struct dirent* dir = readdir((DIR*)fd);
		if (!dir) {
			ThrowErrno();
			return NULL;
		}
		if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
			return ReadDirectory(fd); // Not interested in this crap
		return AutoAlloc<FilesystemNode>(*dir);
	}
	
	size_t PosixFilesystem::TellDirectory(FileHandle fd)
	{
#ifdef BRICKS_FEATURE_ANDROID
		throw NotSupportedException();
#else
		long ret = telldir((DIR*)fd);
		if (ret < 0)
			ThrowErrno();
		return ret;
#endif
	}

	void PosixFilesystem::SeekDirectory(FileHandle fd, size_t offset)
	{
#ifdef BRICKS_FEATURE_ANDROID
		throw NotSupportedException();
#else
		seekdir((DIR*)fd, offset);
#endif
	}

	void PosixFilesystem::CloseDirectory(FileHandle fd)
	{
		if (closedir((DIR*)fd))
			ThrowErrno();
	}

	FileInfo& PosixFilesystem::Stat(const String& path) const
	{
		struct stat st;
		if (stat(path.CString(), &st))
			ThrowErrno();
		return AutoAlloc<FileInfo>(st, FilePath(path).RootPath(GetCurrentDirectory()), const_cast<PosixFilesystem*>(this));
	}
	
	bool PosixFilesystem::IsFile(const String& path) const
	{
		struct stat st;
		return !stat(path.CString(), &st) && S_ISREG(st.st_mode);
	}

	bool PosixFilesystem::IsDirectory(const String& path) const
	{
		struct stat st;
		return !stat(path.CString(), &st) && S_ISDIR(st.st_mode);
	}

	bool PosixFilesystem::Exists(const String& path) const
	{
		struct stat st;
		return !stat(path.CString(), &st);
	}

	const String& PosixFilesystem::GetCurrentDirectory() const
	{
		char buffer[PATH_MAX];
		if (!getcwd(buffer, sizeof(buffer)))
			ThrowErrno();
		return AutoAlloc<String>(buffer);
	}

	void PosixFilesystem::DeleteFile(const String& path)
	{
		if (unlink(path.CString()))
			ThrowErrno();
	}

	void PosixFilesystem::DeleteDirectory(const String& path, bool recursive)
	{
		if (recursive) {
			FilesystemNode node(path);
			foreach (FileNode& subnode, node) {
				if (subnode.GetType() == FileType::Directory)
					DeleteDirectory(subnode.GetFullName(), true);
				else
					DeleteFile(path);
			}
		}
		if (rmdir(path.CString()))
			ThrowErrno();
	}
	
	Stream& FilesystemNode::OpenStream(FileOpenMode::Enum createmode, FileMode::Enum mode, FilePermissions::Enum permissions)
	{
		return AutoAlloc<FileStream>(GetFullName(), createmode, mode, permissions, filesystem);
	}
} }
