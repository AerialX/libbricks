#include "bricks/io/console.h"
#include "bricks/io/stream.h"
#include "bricks/io/filestream.h"
#include "bricks/io/streamreader.h"
#include "bricks/io/streamwriter.h"

#if BRICKS_ENV_MINGW
#include <stdio.h>
#else
#include <unistd.h>
#endif

namespace Bricks { namespace IO {
	AutoPointer<Console> defaultConsole;

	Console* Console::GetDefault()
	{
		if (!defaultConsole)
			defaultConsole = autonew StandardConsole();
		return defaultConsole;
	}

	Console::Console(Stream* in, Stream* out, Stream* error) :
		In(autonew StreamReader(in)), Out(autonew StreamWriter(out)), Error(autonew StreamWriter(error))
	{

	}

	Console::~Console()
	{

	}

	StandardConsole::StandardConsole() : Console(
			autonew FileStream(STDIN_FILENO),
			autonew FileStream(STDOUT_FILENO),
			autonew FileStream(STDERR_FILENO)
		)
	{

	}
} }
