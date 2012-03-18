#include "bricks/io/streamnavigator.h"
#include "bricks/io/stream.h"
#include "bricks/core/math.h"

namespace Bricks { namespace IO {
	StreamNavigator::StreamNavigator(Stream* stream, Endian::Enum endianness) :
		stream(stream), endianness(endianness)
	{

	}

	StreamNavigator::~StreamNavigator()
	{

	}

	void StreamNavigator::PadTo(u64 position)
	{
		if (position < stream->GetPosition())
			BRICKS_FEATURE_THROW(InvalidArgumentException("position"));
		Pad(position - stream->GetPosition());
	}

	void StreamNavigator::PadToMultiple(int round)
	{
		PadTo(Math::RoundUp(stream->GetPosition(), round));
	}

	u64 StreamNavigator::GetPosition()
	{
		return stream->GetPosition();
	}

	void StreamNavigator::SetPosition(u64 position)
	{
		stream->SetPosition(position);
	}

	u64 StreamNavigator::GetLength()
	{
		return stream->GetLength();
	}

	bool StreamNavigator::IsEndOfFile()
	{
		return GetPosition() == GetLength();
	}
} }
