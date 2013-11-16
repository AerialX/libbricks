#pragma once

#include "bricks/core/exception.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#if BRICKS_ENV_MINGW
#define S_IRWXG	00070
#define S_IRGRP	00040
#define S_IWGRP	00020
#define S_IXGRP	00100
#define S_IRWXO	00007
#define S_IROTH	00004
#define S_IWOTH	00002
#define S_IXOTH	00001
#define S_ISVTX	0001000
#define S_ISUID	0004000
#define S_ISGID	0002000

#define S_ISLNK(m)	0
#define S_ISSOCK(m)	0

#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12
#define DT_WHT		14

typedef short nlink_t;
typedef short uid_t;
typedef short gid_t;
typedef short blksize_t;
typedef short blkcnt_t;

#include <errno.h>
static inline int fsync(int fd) { errno = ENOSYS; return -1; }
#endif

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
		FileNotFoundException(const String& path) : Exception(path) { }
	};
} }
