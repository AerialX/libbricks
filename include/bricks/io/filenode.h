#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

#include <dirent.h>
#include <sys/stat.h>

#include "bricks/collections.h"

namespace Bricks { namespace IO {
	namespace FileType { enum Enum {
		Unknown = DT_UNKNOWN,
		File = DT_REG,
		Directory = DT_DIR,
		BlockDevice = DT_BLK,
		CharacterDevice = DT_CHR,
		FIFO = DT_FIFO,
		SymbolicLink = DT_LNK,
		Socket = DT_SOCK
	}; }
	
	static inline FileType::Enum FileStatType(mode_t mode) {
		if (S_ISREG(mode))
			return FileType::File;
		if (S_ISDIR(mode))
			return FileType::Directory;
		if (S_ISCHR(mode))
			return FileType::CharacterDevice;
		if (S_ISBLK(mode))
			return FileType::BlockDevice;
		if (S_ISFIFO(mode))
			return FileType::FIFO;
		if (S_ISLNK(mode))
			return FileType::SymbolicLink;
		if (S_ISSOCK(mode))
			return FileType::Socket;
		return FileType::Unknown;
	}

	class FileNode : public Object, public Bricks::Collections::Iterable<FileNode>
	{
	private:
		FileType::Enum type;
		CopyPointer<FilePath> path;

	public:
		FileNode() : type(FileType::Unknown) { }
		FileNode(FileType::Enum type, const String& path) : type(type), path(AutoPointer<FilePath>(alloc FilePath(path), false)) { }
		virtual ~FileNode() { }

		virtual FileType::Enum GetType() const { return type; }
		virtual const String& GetName() const { return FilePath(*path).GetFileName(); }
		virtual const String& GetFullName() const { if (!FilePath(*path).IsPathRooted()) throw NotSupportedException(); return *path; }
		virtual Pointer<FileNode> GetParent() const = 0;
		virtual u64 GetSize() const = 0;
		virtual Stream& OpenStream(
			FileOpenMode::Enum createmode = FileOpenMode::Create,
			FileMode::Enum mode = FileMode::ReadWrite,
			FilePermissions::Enum permissions = FilePermissions::OwnerReadWrite
		) = 0;
	};
} }
