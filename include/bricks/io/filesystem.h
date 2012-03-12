#pragma once

#include "bricks/core/object.h"
#include "bricks/core/returnpointer.h"
#include "bricks/io/filepath.h"
#include "bricks/io/filenode.h"

namespace Bricks { namespace IO {
	class Stream;
	struct FileInfo;

	typedef size_t FileHandle;

	class Filesystem : public Object
	{
	public:
		static Filesystem* GetDefault();
		static void SetDefault(Filesystem* filesystem);

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
		virtual FileHandle Duplicate(FileHandle fd) = 0;

		virtual FileHandle OpenDirectory(const String& path) = 0;
		virtual ReturnPointer<FileNode> ReadDirectory(FileHandle fd) = 0;
		virtual size_t TellDirectory(FileHandle fd) = 0;
		virtual void SeekDirectory(FileHandle fd, size_t offset) = 0;
		virtual void CloseDirectory(FileHandle fd) = 0;
		
		virtual FileInfo Stat(const String& path) = 0;
		virtual FileInfo FileStat(FileHandle fd) = 0;

		virtual bool IsFile(const String& path) const = 0;
		virtual bool IsDirectory(const String& path) const = 0;
		virtual bool Exists(const String& path) const = 0;

		virtual void DeleteFile(const String& path) = 0;
		virtual void DeleteDirectory(const String& path, bool recursive) = 0;

		virtual void CreateFile(const String& path, FilePermissions::Enum permissions = FilePermissions::OwnerReadWrite) = 0;
		virtual void CreateDirectory(const String& path, FilePermissions::Enum permissions = FilePermissions::OwnerReadWriteExecute) = 0;

		virtual String GetCurrentDirectory() const = 0;
		virtual void ChangeCurrentDirectory(const String& path) = 0;
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
		FileHandle Duplicate(FileHandle fd);
		
		FileHandle OpenDirectory(const String& path);
		ReturnPointer<FileNode> ReadDirectory(FileHandle fd);
		size_t TellDirectory(FileHandle fd);
		void SeekDirectory(FileHandle fd, size_t offset);
		void CloseDirectory(FileHandle fd);
		
		FileInfo Stat(const String& path);
		FileInfo FileStat(FileHandle fd);

		bool IsFile(const String& path) const;
		bool IsDirectory(const String& path) const;
		bool Exists(const String& path) const;

		void DeleteFile(const String& path);
		void DeleteDirectory(const String& path, bool recursive);

		void CreateFile(const String& path, FilePermissions::Enum permissions);
		void CreateDirectory(const String& path, FilePermissions::Enum permissions);

		String GetCurrentDirectory() const;
		void ChangeCurrentDirectory(const String& path);
	};
	
	struct FileInfo : public Object
	{
	private:
		struct stat st;
		FilePath path;
		AutoPointer<Filesystem> filesystem;

	public:
		FileInfo(const struct stat& st, const String& path, Filesystem* filesystem) :
			st(st), path(path), filesystem(filesystem)
		{ }

		dev_t GetDeviceID() const { return st.st_dev; }
		ino_t GetInode() const { return st.st_ino; }
		nlink_t GetLinkCount() const { return st.st_nlink; }
		uid_t GetUserID() const { return st.st_uid; }
		gid_t GetGroupID() const { return st.st_gid; }
		dev_t GetSpecialDeviceID() const { return st.st_rdev; }
		off_t GetSize() const { return st.st_size; }
		time_t GetAccessTime() const { return st.st_atime; }
		time_t GetModifiedTime() const { return st.st_mtime; }
		time_t GetChangeTime() const { return st.st_ctime; }
#ifdef BRICKS_FEATURE_LINUXBSD
		blksize_t GetBlockSize() const { return st.st_blksize; }
		blkcnt_t GetBlockCount() const { return st.st_blocks; }
#else
		blksize_t GetBlockSize() const { BRICKS_FEATURE_THROW(NotSupportedException()); }
		blkcnt_t GetBlockCount() const { BRICKS_FEATURE_THROW(NotSupportedException()); }
#endif
		FileType::Enum GetFileType() const { return FileStatType(st.st_mode); }
		FilePath GetFilePath() const { return path; }

		FileInfo GetParent() const { return filesystem->Stat(path.GetDirectory()); }
		ReturnPointer<Filesystem> GetFilesystem() const { return filesystem; }
	};

	class FilesystemNodeIterator;
	class FilesystemNode : public FileNode, public Collections::IterableFast<FilesystemNodeIterator>
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

		FilesystemNode(const struct dirent& dir, Filesystem* filesystem = NULL) :
#ifdef BRICKS_FEATURE_LINUXBSD
			FileNode(GetDirType(dir.d_type), dir.d_name),
#else
			FileNode(GetDirType(DT_UNKNOWN), dir.d_name),
#endif
			filesystem(filesystem ?: Filesystem::GetDefault())
		{
			size = -1;
		}

		FilesystemNode(const String& path, Filesystem* filesystem = NULL) :
			FileNode(FileType::Unknown, path), filesystem(filesystem ?: Filesystem::GetDefault())
		{
			if (this->filesystem->Exists(path))
				*this = this->filesystem->Stat(path);
		}

		String GetFullPath() const { if (!path.IsPathRooted()) return path.RootPath(filesystem->GetCurrentDirectory()); return path; }
		ReturnPointer<FileNode> GetParent() const { return autonew FilesystemNode(FilePath(GetFullPath()).GetDirectory(), filesystem); }
		ReturnPointer<FileNode> GetLeaf(const String& path) const { return autonew FilesystemNode(FilePath(GetPath()).Combine(path), filesystem); }
		void CreateFile(FilePermissions::Enum permissions) { filesystem->CreateFile(GetPath(), permissions); }
		void CreateDirectory(FilePermissions::Enum permissions) { filesystem->CreateDirectory(GetPath(), permissions); }
		bool Exists() const { return filesystem->Exists(GetPath()); }
		u64 GetSize() const { if (GetType() == FileType::File && size != (u64)-1) return size; BRICKS_FEATURE_THROW(NotSupportedException()); }
		ReturnPointer<Stream> OpenStream(FileOpenMode::Enum createmode = FileOpenMode::Open, FileMode::Enum mode = FileMode::ReadWrite, FilePermissions::Enum permissions = FilePermissions::OwnerReadWrite);

		ReturnPointer<Bricks::Collections::Iterator<FileNode*> > GetIterator() const;
		FilesystemNodeIterator GetIteratorFast() const;
	};

	class FilesystemNodeIterator : public Bricks::Collections::Iterator<FileNode*>
	{
	private:
		AutoPointer<Filesystem> filesystem;
		FileHandle dir;
		AutoPointer<FileNode> current;
		mutable FileNode* currentRef;

		friend class FilesystemNode;

	public:
		FilesystemNodeIterator(const FilesystemNode* node) :
			filesystem(node->filesystem), dir(filesystem->OpenDirectory(node->GetPath())) { }
		~FilesystemNodeIterator() { filesystem->CloseDirectory(dir); }

		FileNode*& GetCurrent() const { if (!current) BRICKS_FEATURE_THROW(Bricks::Collections::InvalidIteratorException()); return (currentRef = current); }
		bool MoveNext() { return (current = filesystem->ReadDirectory(dir)); }
	};
	
	inline ReturnPointer<Bricks::Collections::Iterator<FileNode*> > FilesystemNode::GetIterator() const { return autonew FilesystemNodeIterator(this); }
	inline FilesystemNodeIterator FilesystemNode::GetIteratorFast() const { return FilesystemNodeIterator(this); }
} }
