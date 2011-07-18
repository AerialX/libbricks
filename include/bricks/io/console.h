#pragma once

#include "bricks/object.h"
#include "bricks/io/stream.h"
#include "bricks/io/navigator.h"
#include "bricks/io/filestream.h"

#include <unistd.h>

namespace Bricks { namespace IO {
	class Console : public Object
	{
	protected:
		Console(Stream& in, Stream& out, Stream& error) : In(TempAlloc<StreamReader>(in)), Out(TempAlloc<StreamWriter>(out)), Error(TempAlloc<StreamWriter>(error)) { }

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
				TempAlloc<FileStream>(STDIN_FILENO),
				TempAlloc<FileStream>(STDOUT_FILENO),
				TempAlloc<FileStream>(STDERR_FILENO)
		) { }
	};
} }
