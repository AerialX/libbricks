#pragma once

#include <sys/stat.h>

#include "bricks.h"
#include "bricks/io/types.h"

namespace Bricks { namespace IO {
	class FileInfo : public Object
	{
	public:
		FileInfo(struct stat& st) { }
	};

	class DirectoryInfo : public Object
	{
	public:
		DirectoryInfo(struct stat& st) { }
	};
} }
