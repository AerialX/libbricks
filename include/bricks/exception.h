#pragma once

#include "bricks/string.h"

namespace Bricks {
	void ThrowErrno();

	class Exception
	{
	protected:
		String Message;

	public:
		Exception() { }
		Exception(const String& message) : Message(message) { }

		String GetMessage() { return Message; }
	};

	class InvalidArgumentException : public Exception
	{
	protected:
		String Argument;

	public:
		InvalidArgumentException(const String& argument, const String& message = "") : Exception(message), Argument(argument) { }

		String GetArgument() { return Argument; }
	};

	class NotImplementedException : public Exception
	{
	public:
		NotImplementedException(const String& message = "") : Exception(message) { }
	};
}
