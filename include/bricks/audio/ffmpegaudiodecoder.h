#pragma once

#include "bricks/config.h"

#if BRICKS_CONFIG_AUDIO_FFMPEG

#include "bricks/audio/ffmpegdecoder.h"
#include "bricks/audio/audiocodec.h"

namespace Bricks { namespace Audio {
	class FFmpegAudioDecoder : public AudioCodec<s16>, NoCopy
	{
	protected:
		AutoPointer<FFmpegDecoder> decoder;
		size_t streamIndex;
		AVStream* stream;
		AVCodec* codec;
		void* buffer;
		size_t bufferSize;
		void* cache;
		size_t cacheLength;

		int ReadCache(AudioBuffer<s16>& buffer, int count, int offset);
		void Initialize();

	public:
		FFmpegAudioDecoder(FFmpegDecoder* decoder);
		FFmpegAudioDecoder(IO::Stream* stream);
		~FFmpegAudioDecoder();

		void Seek(s64 sample);
		u32 Read(AudioBuffer<s16>& buffer, u32 count, u32 offset = 0);
	};
} }

#endif
