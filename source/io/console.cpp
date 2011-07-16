#include "bricksall.hpp"

namespace Bricks { namespace IO {
	Console& Console::Default = Alloc<StandardConsole>();
} }
