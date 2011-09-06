#pragma once

#include "bricks/config.h"

#ifdef BRICKS_CONFIG_IMAGING_LIBPNG

#include "bricks/imaging/image.h"
#include "bricks/io/filesystem.h"
#include "bricks/io/filestream.h"

namespace Bricks { namespace Imaging {
	namespace PNG {
		ReturnPointer<Image> LoadImage(const Pointer<IO::Stream>& stream, bool transform = false);
		static inline ReturnPointer<Image> LoadImage(const String& path, const Pointer<IO::Filesystem>& filesystem = NULL, bool transform = false) { return LoadImage(autonew IO::FileStream(path, IO::FileOpenMode::Open, IO::FileMode::ReadOnly, IO::FilePermissions::OwnerReadWrite, filesystem), transform); }
	}
} }

#endif
