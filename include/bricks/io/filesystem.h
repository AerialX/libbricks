#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

#include <stdio.h>

namespace Bricks { namespace IO {
	class FileInfo;

	typedef size_t FileHandle;

	class Filesystem : public virtual Object
	{
	public:
		static const Pointer<Filesystem>& GetDefault();

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
		
		virtual FileInfo& Stat(const String& path) const = 0;

		virtual bool IsFile(const String& path) const = 0;
		virtual bool IsDirectory(const String& path) const = 0;
		virtual bool Exists(const String& path) const = 0;

		virtual void DeleteFile(const String& path) = 0;
		virtual void DeleteDirectory(const String& path, bool recursive) = 0;

		virtual const String& GetCurrentDirectory() const = 0;
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
		
		FileInfo& Stat(const String& path) const;

		bool IsFile(const String& path) const;
		bool IsDirectory(const String& path) const;
		bool Exists(const String& path) const;

		void DeleteFile(const String& path);
		void DeleteDirectory(const String& path, bool recursive);

		const String& GetCurrentDirectory() const;
	};
	
	class FileInfo : public virtual Object
	{
	private:
		struct stat st;
		AutoPointer<FilePath> path;
		AutoPointer<Filesystem> filesystem;

	public:
		FileInfo(struct stat& st, const String& path, Pointer<Filesystem> filesystem) :
			st(st), path(alloc FilePath(path), false), filesystem(filesystem)
		{ }
		virtual ~FileInfo() { }

		dev_t GetDeviceID() const { return st.st_dev; }
		ino_t GetInode() const { return st.st_ino; }
		nlink_t GetLinkCount() const { return st.st_nlink; }
		uid_t GetUserID() const { return st.st_uid; }
		gid_t GetGroupID() const { return st.st_gid; }
		dev_t GetSpecialDeviceID() const { return st.st_rdev; }
		off_t GetSize() const { return st.st_size; }
		blksize_t GetBlockSize() const { return st.st_blksize; }
		blkcnt_t GetBlockCount() const { return st.st_blocks; }
		time_t GetAccessTime() const { return st.st_atime; }
		time_t GetModifiedTime() const { return st.st_mtime; }
		time_t GetChangeTime() const { return st.st_ctime; }
		FileType::Enum GetFileType() const { return FileStatType(st.st_mode); }
		const FilePath& GetFilePath() const { return *path; }

		FileInfo& GetParent() const { return filesystem->Stat(path->GetDirectory()); }
		Filesystem& GetFilesystem() const { return const_cast<Filesystem&>(*filesystem); }
	};

	class FilesystemNodeIterator;
	class FilesystemNode : public FileNode
	{
	private:
		AutoPointer<Filesystem> filesystem;
		u64 size;

		static FileType::Enum GetDirType(int type) {
			return (FileType::Enum)type;
		}

		friend class FilesystemNodeIterator;

	public:
		FilesystemNode(const FileInfo& info) : FileNode(info.GetFileType(), info.GetFilePath()), filesystem(info.GetFilesystem()) {
			size = info.GetSize();
		}

		FilesystemNode(const struct dirent& dir, Pointer<Filesystem> filesystem = NULL) :
			FileNode(GetDirType(dir.d_type), dir.d_name),
			filesystem(filesystem ? filesystem.GetValue() : Filesystem::GetDefault())
		{
			size = -1;
		}

		FilesystemNode(const String& path, Pointer<Filesystem> filesystem = NULL) :
			filesystem(filesystem ? filesystem : Filesystem::GetDefault())
		{
			self = this->filesystem->Stat(path);
		}

		Pointer<FileNode> GetParent() const { return autoalloc FilesystemNode(FilePath(GetFullName()).GetDirectory(), filesystem); }
		u64 GetSize() const { if (GetType() == FileType::File && size != (u64)-1) return size; throw NotSupportedException(); }
		Stream& OpenStream(FileOpenMode::Enum createmode, FileMode::Enum mode, FilePermissions::Enum permissions);

		Bricks::Collections::Iterator<FileNode>& GetIterator() const;
	};

	class FilesystemNodeIterator : public virtual Object, public Bricks::Collections::Iterator<FileNode>
	{
	private:
		AutoPointer<Filesystem> filesystem;
		FileHandle dir;
		AutoPointer<FileNode> current;

		FilesystemNodeIterator(const FilesystemNode& node) :
			filesystem(node.filesystem), dir(filesystem->OpenDirectory(node.GetFullName())) { }

		friend class FilesystemNode;

	public:
		~FilesystemNodeIterator() { filesystem->CloseDirectory(dir); }

		FileNode& GetCurrent() const { if (!current) throw Bricks::Collections::InvalidIteratorException(); return const_cast<FileNode&>(*current); }
		bool MoveNext() { return (current = filesystem->ReadDirectory(dir)); }
	};
	
	inline Bricks::Collections::Iterator<FileNode>& FilesystemNode::GetIterator() const { return autoalloc FilesystemNodeIterator(self); }
} }
