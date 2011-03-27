#pragma once

#include "bricks/io.h"

#include <unistd.h>

namespace Bricks { namespace IO {
	class Console : public Object
	{
	protected:
		Console(Stream& in, Stream& out, Stream& error) : In(Alloc(StreamReader, in)), Out(Alloc(StreamWriter, out)), Error(Alloc(StreamWriter, error)) { }

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
				*AutoPointer<FileStream>(Alloc(FileStream, STDIN_FILENO), false),
				*AutoPointer<FileStream>(Alloc(FileStream, STDOUT_FILENO), false),
				*AutoPointer<FileStream>(Alloc(FileStream, STDERR_FILENO), false)
		) { }
	};
} }
