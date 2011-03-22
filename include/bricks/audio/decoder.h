#pragma once

namespace Bricks { namespace Audio {
	class Decoder
	{
	protected:

	public:
		u8 Channels;
		u32 SampleRate;
		u32 Length;
		u32 Position;
		
		virtual void Seek(u32 position);
		virtual u32 Read(AudioBuffer& buffer, u32 length = 0);

		virtual ~Decoder();
	};
} }
