#include "bricks/io.h"

namespace Bricks { namespace IO {
	Console& Console::Default = Alloc(StandardConsole);
} }
