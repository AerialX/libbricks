#include "bricksall.hpp"

#ifdef BRICKS_FEATURE_APPLE
#define memalign(a, b) malloc(b)
#else
#include <malloc.h>
#endif

using namespace Bricks;
using namespace Bricks::Audio;

BRICKS_FEATURE_CONSTRUCTOR(FFmpegInit)
static void FFmpegInit()
{
	av_register_all();
}

FFmpegDecoder::FFmpegDecoder(const String& filename)
{
	if (av_open_input_file(&format, filename.CString(), NULL, 0, NULL) < 0)
		throw Exception();
	if (av_find_stream_info(format) < 0)
		throw FormatException();
	for (streamIndex = 0; streamIndex < format->nb_streams; streamIndex++) {
		stream = format->streams[streamIndex];
		if (!stream)
			continue;
		if (stream->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			codec = avcodec_find_decoder(stream->codec->codec_id);
			break;
		}
	}
	if (!codec)
		throw FormatException();

	avcodec_open(stream->codec, codec);
	channels = stream->codec->channels;
	samplerate = stream->codec->sample_rate;
	if (stream->duration < 0)
		samples = INT64_MAX;
	else
		samples = stream->time_base.num * stream->duration * samplerate / stream->time_base.den;

	bufferSize = AVCODEC_MAX_AUDIO_FRAME_SIZE;
	buffer = memalign(0x20, bufferSize);

	cache = memalign(0x20, bufferSize);
	cacheLength = 0;
}

FFmpegDecoder::~FFmpegDecoder()
{
	free(cache);
	free(buffer);
	avcodec_close(stream->codec);
	av_close_input_file(format);
}

void FFmpegDecoder::Seek(s64 sample)
{
	cacheLength = 0;
	av_seek_frame(format, streamIndex, stream->time_base.den * sample / stream->time_base.num / samplerate, AVSEEK_FLAG_ANY);
	AudioCodec<s16>::Seek(sample);
}

int FFmpegDecoder::ReadCache(AudioBuffer<s16>& buffer, int count, int offset)
{
	count = BRICKS_FEATURE_MIN((size_t)count, cacheLength / sizeof(AudioBuffer<s16>::AudioSample) / channels);
	buffer.DeinterlaceFrom((AudioBuffer<s16>::AudioSample*)cache, count, offset);
	cacheLength -= sizeof(AudioBuffer<s16>::AudioSample) * channels * count;
	memmove(cache, (u8*)cache + sizeof(AudioBuffer<s16>::AudioSample) * channels * count, cacheLength);
	return count;
}

u32 FFmpegDecoder::Read(AudioBuffer<s16>& buffer, u32 count, u32 boffset)
{
	int offset = ReadCache(buffer, count, boffset);
	count -= offset;
	while (count > 0) {
		if (av_read_frame(format, &packet) < 0)
			break;

		cacheLength = 0;

		while (packet.size > 0) {
			int datasize = bufferSize;
			int used = avcodec_decode_audio3(stream->codec, (s16*)this->buffer, &datasize, &packet);
			if (used < 0)
				break;
			packet.size -= used;
			packet.data += used;

			if (datasize <= 0)
				break;

			int read = BRICKS_FEATURE_MAX(BRICKS_FEATURE_MIN(datasize, count * 2 * channels), 0);
			int left = datasize - read;
			if (read > 0) {
				int samples = read / sizeof(AudioBuffer<s16>::AudioSample) / channels;
				buffer.DeinterlaceFrom((AudioBuffer<s16>::AudioSample*)this->buffer, samples, boffset + offset);
				offset += samples;
				count -= samples;
			}

			if (left > 0) {
				memcpy((u8*)cache + cacheLength, (u8*)this->buffer + read, left);
				cacheLength += left;
			}
		}
	}
	AudioCodec<s16>::Read(buffer, offset);
	return offset;
}
