#pragma once

#include "bricks/core/autopointer.h"

namespace Bricks { namespace IO {
	class StreamReader;
	class StreamWriter;
	class Stream;

	class Console : public Object
	{
	protected:
		Console(Stream* in, Stream* out, Stream* error);

	public:
		~Console();

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
