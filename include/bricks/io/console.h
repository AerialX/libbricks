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
		Console(Pointer<Stream> in, Pointer<Stream> out, Pointer<Stream> error) : In(autonew StreamReader(in)), Out(autonew StreamWriter(out)), Error(autonew StreamWriter(error)) { }

	public:
		static Pointer<Console> Default;

		AutoPointer<StreamReader> In;
		AutoPointer<StreamWriter> Out;
		AutoPointer<StreamWriter> Error;

		virtual ~Console() { }
	};

	class StandardConsole : public Console
	{
	public:
		StandardConsole() : Console(
				autonew FileStream(STDIN_FILENO),
				autonew FileStream(STDOUT_FILENO),
				autonew FileStream(STDERR_FILENO)
		) { }
	};
} }
