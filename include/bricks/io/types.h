#pragma once

#include <bricks/types.h>
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
} }
