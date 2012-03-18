#pragma once

#include "bricks/config.h"

#if BRICKS_CONFIG_AUDIO_FFMPEG

#include "bricks/core/copypointer.h"
#include "bricks/collections/array.h"

namespace Bricks { namespace IO { class Stream; } }

extern "C" {
	struct AVFormatContext;
	struct AVStream;
	struct AVCodec;
	struct AVPacket;
	struct AVFrame;
}

namespace Bricks { namespace Audio {
	class FFmpegDecoder : public Object, NoCopy
	{
	protected:
		size_t streamIndex;
		AVFormatContext* format;
		AutoPointer<IO::Stream> ioStream;
		void* ffmpegBuffer;
		static const int ffmpegBufferSize = 0x1000;

		bool currentTimeKnown;
		s64 currentTime;

		Bricks::Collections::Array<AVPacket> packetQueue;
		Bricks::Collections::Array<int> openStreams;

	public:
		FFmpegDecoder(IO::Stream* stream);
		~FFmpegDecoder();

		AVFormatContext* GetFormatContext() const { return format; }
		IO::Stream* GetStream() const { return ioStream; }

		AVCodec* OpenStream(int streamIndex);
		void CloseStream(int streamIndex);
		bool IsStreamOpen(int streamIndex);

		bool ReadPacket(AVPacket* packet, int streamIndex = -1);
		void FreePacket(AVPacket* packet);

		void QueuePacket(AVPacket* packet);
		bool DequeuePacket(AVPacket* packet, int streamIndex = -1);
		void FlushQueue();

		void SeekFrame(s64 time);
		s64 GetFrameTime();
		void ResetFrameTime();
	};
} }

#endif
