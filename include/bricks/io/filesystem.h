#pragma once

#include "bricks.h"
#include "bricks/io/types.h"
#include "bricks/io/filenode.h"
#include "bricks/io/fileinfo.h"

#include <stdio.h>

namespace Bricks { namespace IO {
	typedef size_t FileHandle;

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

		virtual FileHandle OpenDirectory(const String& path) = 0;
		virtual Pointer<FileNode> ReadDirectory(FileHandle fd) = 0;
		virtual size_t TellDirectory(FileHandle fd) = 0;
		virtual void SeekDirectory(FileHandle fd, size_t offset) = 0;
		virtual void CloseDirectory(FileHandle fd) = 0;
		
		virtual FileInfo& StatFile(const String& path) = 0;
		virtual DirectoryInfo& StatDirectory(const String& path) = 0;

		virtual void DeleteFile(const String& path) = 0;
		virtual void DeleteDirectory(const String& path, bool recursive) = 0;
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
		
		FileHandle OpenDirectory(const String& path);
		Pointer<FileNode> ReadDirectory(FileHandle fd);
		size_t TellDirectory(FileHandle fd);
		void SeekDirectory(FileHandle fd, size_t offset);
		void CloseDirectory(FileHandle fd);
		
		FileInfo& StatFile(const String& path);
		DirectoryInfo& StatDirectory(const String& path);

		void DeleteFile(const String& path);
		void DeleteDirectory(const String& path, bool recursive);
	};

	class FilesystemNode : public FileNode
	{
	private:
		AutoPointer<Filesystem> filesystem;

		static NodeType::Enum GetStatType(mode_t mode) {
			if (S_ISREG(mode))
				return NodeType::File;
			if (S_ISDIR(mode))
				return NodeType::Directory;
			if (S_ISCHR(mode))
				return NodeType::CharacterDevice;
			if (S_ISBLK(mode))
				return NodeType::BlockDevice;
			if (S_ISFIFO(mode))
				return NodeType::FIFO;
			if (S_ISLNK(mode))
				return NodeType::SymbolicLink;
			if (S_ISSOCK(mode))
				return NodeType::Socket;
			return NodeType::Unknown;
		}

		static NodeType::Enum GetDirType(int type) {
			return (NodeType::Enum)type;
		}

	public:
		FilesystemNode(const struct stat& st, const String& path, Pointer<Filesystem> filesystem = NULL) :
			FileNode(GetStatType(st.st_mode), path), // TODO: FilePath::GetLeaf(path), FilePath::GetDirectory(path)
			filesystem(filesystem ?: &Filesystem::GetDefault())
		{

		}

		FilesystemNode(const struct dirent& dir, Pointer<Filesystem> filesystem = NULL) :
			FileNode(GetDirType(dir.d_type), dir.d_name),
			filesystem(filesystem ?: &Filesystem::GetDefault())
		{

		}
		FilesystemNode(const String& path, Pointer<Filesystem> filesystem = NULL) :
			filesystem(filesystem ?: &Filesystem::GetDefault())
		{
			Throw(NotImplementedException);
//			self = filesystem->Stat(path);
		}

		const String& GetFullName() const { Throw(NotImplementedException); }
		Pointer<FileNode> GetParent() const { Throw(NotImplementedException); }
		u64 GetSize() const { Throw(NotImplementedException); }
		Stream& OpenStream(FileOpenMode::Enum createmode, FileMode::Enum mode, FilePermissions::Enum permissions);

		Bricks::Collections::Iterator<FileNode>& GetIterator() const { Throw(NotImplementedException); }
	};
} }
