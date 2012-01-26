#pragma once

#include "bricks/core/object.h"
#include "bricks/io/navigator.h"

namespace Bricks { namespace IO {
	class Console : public Object
	{
	protected:
		Console(Stream* in, Stream* out, Stream* error);

	public:
		static Console* GetDefault();

		AutoPointer<StreamReader> In;
		AutoPointer<StreamWriter> Out;
		AutoPointer<StreamWriter> Error;
	};

	class StandardConsole : public Console
	{
	public:
		StandardConsole();
	};
} }
