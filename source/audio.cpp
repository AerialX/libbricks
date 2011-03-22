#include "bricks/audio.h"

namespace Bricks { namespace Audio {
	void Decoder::Seek(u32 position)
	{
		Position = position;
	}

	u32 Decoder::Read(AudioBuffer& buffer, u32 length)
	{
		Position += length;
		return length;
	}

	Decoder::~Decoder()
	{

	}
} }
