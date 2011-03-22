#pragma once

namespace Bricks { namespace Audio {
	class AudioBuffer
	{
	protected:

	public:
		s16** Data;
		u8 Channels;
		u32 Length;

		AudioBuffer(u8 channels, u32 length);

		const void MixTo(AudioBuffer& buffer);
		const void CopyTo(AudioBuffer& buffer);

		const void Interlace(s16* data, u32 length, u32 offset = 0);
		void Deinterlace(const s16* data, u32 length, u32 offset = 0);
	};
} }
