#ifdef BRICKS_FEATURE_FFMPEG

#include "bricks/audio/audiocodec.h"

namespace Bricks { namespace Audio {
	extern "C" {
		#include <libavcodec/avcodec.h>
		#include <libavformat/avformat.h>
	}

	class FFmpegDecoder : public AudioCodec<short>
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

		int ReadCache(AudioBuffer<short>& buffer, int count, int offset);

	public:
		FFmpegDecoder(const String& filename);
		~FFmpegDecoder();

		void Seek(s64 sample);
		int Read(AudioBuffer<short>& buffer, int count, int offset = 0);
	};
} }

#endif
