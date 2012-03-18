#pragma once

#include "bricks/config.h"

#if BRICKS_CONFIG_IMAGING_LIBPNG

#include "bricks/core/returnpointer.h"

namespace Bricks { namespace IO {
	class Stream;
	class Filesystem;
} }

namespace Bricks { namespace Imaging {
	class Bitmap;

	namespace PNG {
		ReturnPointer<Bitmap> LoadImage(IO::Stream* stream, bool transform = false);
		ReturnPointer<Bitmap> LoadImage(const String& path, bool transform = false, IO::Filesystem* filesystem = NULL);
	}
} }

#endif
