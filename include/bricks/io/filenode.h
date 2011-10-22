#pragma once

#include <dirent.h>
#include <sys/stat.h>

#include "bricks/io/types.h"
#include "bricks/io/filepath.h"
#include "bricks/collections/iterator.h"

namespace Bricks { namespace IO {
	class Stream;

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

	class FileNode : public Object, public Collections::Iterable<FileNode>
	{
	protected:
		FileType::Enum type;
		FilePath path;

	public:
		FileNode() : type(FileType::Unknown) { }
		FileNode(FileType::Enum type, const String& path) : type(type), path(path) { }

		virtual FileType::Enum GetType() const { return type; }
		virtual String GetPath() const { return path; }
		virtual String GetName() const { return path.GetFileName(); }
		virtual String GetFullPath() const { if (!path.IsPathRooted()) throw NotSupportedException(); return path; }
		virtual ReturnPointer<FileNode> GetParent() const = 0;
		virtual u64 GetSize() const = 0;
		virtual ReturnPointer<Stream> OpenStream(
			FileOpenMode::Enum createmode = FileOpenMode::Open,
			FileMode::Enum mode = FileMode::ReadWrite,
			FilePermissions::Enum permissions = FilePermissions::OwnerReadWrite
		) = 0;
	};
} }
