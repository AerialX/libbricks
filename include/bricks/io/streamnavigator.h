#pragma once

#include "bricks/core/autopointer.h"
#include "bricks/core/exception.h"
#include "bricks/io/endian.h"

namespace Bricks { namespace IO {
	class Stream;

	class EndOfStreamException : public Exception
	{
	public:
		EndOfStreamException(const String& message = String::Empty) : Exception(message) { }
	};
	
	class StreamException : public Exception
	{
	public:
		StreamException(const String& message = String::Empty) : Exception(message) { }
	};
	
	class StreamNavigator : public Object
	{
	protected:
		AutoPointer<Stream> stream;
		Endian::Enum endianness;

	public:
		StreamNavigator(Stream* stream, Endian::Enum endianness);
		~StreamNavigator();

		Stream* GetStream() { return stream; }

		virtual void Pad(u64 size) = 0;

		void PadTo(u64 position);
		void PadToMultiple(int round);

		u64 GetPosition();
		void SetPosition(u64 position);
		u64 GetLength();
		bool IsEndOfFile();
	};
} }
