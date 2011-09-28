#pragma once

#include "bricks/config.h"

#ifdef BRICKS_CONFIG_AUDIO_FFMPEG

#include "bricks/audio/audiocodec.h"
#include "bricks/io/stream.h"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

namespace Bricks { namespace Audio {
	class FFmpegDecoder : public Object, public AudioCodec<s16>, NoCopy
	{
	protected:
		size_t streamIndex;
		AVFormatContext* format;
		AVStream* stream;
		AVCodec* codec;
		AVPacket packet;
		void* buffer;
		size_t bufferSize;
		void* cache;
		size_t cacheLength;
		AutoPointer<IO::Stream> ioStream;
		AVIOContext* ffmpegStream;
		void* ffmpegBuffer;
		static const int ffmpegBufferSize = 0x1000;

		int ReadCache(AudioBuffer<s16>& buffer, int count, int offset);

	public:
		FFmpegDecoder(const Pointer<IO::Stream>& stream);
		~FFmpegDecoder();

		void Seek(s64 sample);
		u32 Read(AudioBuffer<s16>& buffer, u32 count, u32 offset = 0);
	};
} }

#endif
