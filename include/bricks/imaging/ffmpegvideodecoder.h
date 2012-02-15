#pragma once

#include "bricks/config.h"

#ifdef BRICKS_CONFIG_AUDIO_FFMPEG

#include "bricks/audio/ffmpegdecoder.h"
#include "bricks/imaging/videocodec.h"

namespace Bricks { namespace Imaging {
	class FFmpegVideoDecoder : public VideoCodec, NoCopy
	{
	protected:
		AutoPointer<Audio::FFmpegDecoder> decoder;
		size_t streamIndex;
		AVStream* stream;
		AVCodec* codec;
		AVFrame* frame;
		AVPacket* packet;
		AVPacket* originalPacket;

		void Initialize();

	public:
		FFmpegVideoDecoder(Audio::FFmpegDecoder* decoder);
		FFmpegVideoDecoder(IO::Stream* stream);
		~FFmpegVideoDecoder();

		void Seek(s64 frame);
		bool Read(BitmapImage* image);
	};
} }

#endif
