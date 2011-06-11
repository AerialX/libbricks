#pragma once

#include "bricks/io.h"

#include <unistd.h>

namespace Bricks { namespace IO {
	class Console : public virtual Object
	{
	protected:
		Console(Stream& in, Stream& out, Stream& error) : In(alloc StreamReader(in), false), Out(alloc StreamWriter(out), false), Error(alloc StreamWriter(error), false) { }

	public:
		static Console& Default;

		AutoPointer<StreamReader> In;
		AutoPointer<StreamWriter> Out;
		AutoPointer<StreamWriter> Error;

		virtual ~Console() { }
	};

	class StandardConsole : public Console
	{
	public:
		StandardConsole() : Console(
				*AutoPointer<FileStream>(alloc FileStream(STDIN_FILENO), false),
				*AutoPointer<FileStream>(alloc FileStream(STDOUT_FILENO), false),
				*AutoPointer<FileStream>(alloc FileStream(STDERR_FILENO), false)
		) { }
	};
} }
