#pragma once

#include "bricks/core/object.h"
#include "bricks/core/string.h"

namespace Bricks {
	void ThrowErrno();

	class Exception : public Object
	{
	protected:
		String Message;

	public:
		Exception(const String& message = String::Empty) : Message(message) { }

		String GetMessage() const { return Message; }
	};

	class InternalInconsistencyException : public Exception
	{
	public:
		InternalInconsistencyException(const String& message = String::Empty) : Exception(message) { }
	};

	class InvalidArgumentException : public Exception
	{
	protected:
		String Argument;

	public:
		InvalidArgumentException(const String& argument = String::Empty, const String& message = String::Empty) : Exception(message), Argument(argument) { }

		String GetArgument() const { return Argument; }
	};

	class InvalidOperationException : public Exception
	{
	public:
		InvalidOperationException(const String& message = String::Empty) : Exception(message) { }
	};

	class NotImplementedException : public Exception
	{
	public:
		NotImplementedException(const String& message = String::Empty) : Exception(message) { }
	};

	class NotSupportedException : public Exception
	{
	public:
		NotSupportedException(const String& message = String::Empty) : Exception(message) { }
	};

	class NullReferenceException : public Exception
	{
	public:
		NullReferenceException(const String& message = String::Empty) : Exception(message) { }
	};

	class OutOfMemoryException : public Exception
	{
	public:
		OutOfMemoryException(const String& message = String::Empty) : Exception(message) { }
	};

	class FormatException : public Exception
	{
	public:
		FormatException(const String& message = String::Empty) : Exception(message) { }
	};

	class ErrnoException : public Exception
	{
	public:
		int Value;
		ErrnoException(const String& message = String::Empty, int value = 0) : Exception(message), Value(value) { }
	};
}
