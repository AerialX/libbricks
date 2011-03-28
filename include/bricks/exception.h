#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

namespace Bricks {
	void ThrowErrno();

	class Exception : public Object
	{
	protected:
		CopyPointer<String> Message;

	public:
		Exception() : Message(String::Empty) { }
		Exception(const String& message) : Message(message) { }
		virtual ~Exception() { }

		const String& GetMessage() const { return *Message; }
	};

	class InvalidArgumentException : public Exception
	{
	protected:
		CopyPointer<String> Argument;

	public:
		InvalidArgumentException(const String& argument = String::Empty, const String& message = String::Empty) : Exception(message), Argument(argument) { }

		const String& GetArgument() const { return *Argument; }
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

	class ErrnoException : public Exception
	{
	public:
		int Value;
		ErrnoException(const String& message = String::Empty, int value = 0) : Exception(message), Value(value) { }
	};

#ifdef BRICKS_CONFIG_CPP0X
	template<typename T, typename... Args> BRICKS_FEATURE_NORETURN void Throw(Args... args)
	{
		throw T(args...);
	}
#endif
#define Throw(T, ...) do { throw T(__VA_ARGS__); } while (false)
}
