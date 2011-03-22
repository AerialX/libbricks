#pragma once

#include "bricks/string.h"

namespace Bricks { namespace IO {
	class FilePath : public String
	{
	public:
		FilePath(const String& string) : String(string) { }
	};
} }
