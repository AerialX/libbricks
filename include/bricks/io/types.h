#pragma once

#include "bricks/object.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

namespace Bricks { namespace IO {
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
		OwnerExecute	= S_IXUSR,
		OwnerReadWrite	= OwnerRead | OwnerWrite,
		OwnerReadWriteExecute	= OwnerReadWrite | OwnerExecute,
		GroupAll		= S_IRWXG,
		GroupRead		= S_IRGRP,
		GroupWrite		= S_IWGRP,
		GroupExecute	= S_IXGRP,
		GroupReadWrite	= GroupRead | GroupWrite,
		GroupReadWriteExecute	= GroupReadWrite | GroupExecute,
		OtherAll		= S_IRWXO,
		OtherRead		= S_IROTH,
		OtherWrite		= S_IWOTH,
		OtherExecute	= S_IXOTH,
		OtherReadWrite	= OtherRead | OtherWrite,
		OtherReadWriteExecute	= OtherReadWrite | OtherExecute,
		SetUserID		= S_ISUID,
		SetGroupID		= S_ISGID,
		Sticky			= S_ISVTX
	}; }

	namespace SeekType { enum Enum {
		Beginning = SEEK_SET,
		Current = SEEK_CUR,
		End = SEEK_END
	}; }

	class FileNotFoundException : public Exception
	{
	public:
		FileNotFoundException(const String& message = String::Empty) : Exception(message) { }
	};
} }
