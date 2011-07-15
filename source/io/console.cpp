#include "bricks/io/console.h"

namespace Bricks { namespace IO {
	Console& Console::Default = Alloc<StandardConsole>();
} }
