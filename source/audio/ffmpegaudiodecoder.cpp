#include "bricks/audio/ffmpegaudiodecoder.h"

#if BRICKS_CONFIG_AUDIO_FFMPEG
#include "bricks/core/exception.h"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Audio;

namespace Bricks { namespace Audio {
	FFmpegAudioDecoder::FFmpegAudioDecoder(FFmpegDecoder* decoder) :
		decoder(decoder)
	{
		Initialize();
	}

	FFmpegAudioDecoder::FFmpegAudioDecoder(Stream* ioStream) :
		decoder(autonew FFmpegDecoder(ioStream))
	{
		Initialize();
	}

	void FFmpegAudioDecoder::Initialize()
	{
		AVFormatContext* format = decoder->GetFormatContext();
		for (streamIndex = 0; streamIndex < format->nb_streams; streamIndex++) {
			stream = format->streams[streamIndex];
			if (!stream)
				continue;
			if (stream->codec->codec_type == AVMEDIA_TYPE_AUDIO)
				break;
		}

		if (streamIndex >= format->nb_streams)
			BRICKS_FEATURE_THROW(FormatException());

		codec = decoder->OpenStream(streamIndex);

		channels = stream->codec->channels;
		samplerate = stream->codec->sample_rate;
		if (stream->duration < 0)
			samples = INT64_MAX;
		else
			samples = stream->time_base.num * stream->duration * samplerate / stream->time_base.den;

		bufferSize = AVCODEC_MAX_AUDIO_FRAME_SIZE;
		buffer = av_malloc(bufferSize);

		cache = av_malloc(bufferSize);
		cacheLength = 0;

		// TODO: Handle stream->codec->sample_fmt
	}

	FFmpegAudioDecoder::~FFmpegAudioDecoder()
	{
		av_free(cache);
		av_free(buffer);
	}

	void FFmpegAudioDecoder::Seek(s64 sample)
	{
		decoder->SeekFrame(av_rescale(sample, stream->time_base.den, stream->time_base.num) / samplerate);
		cacheLength = 0;
		AudioCodec<s16>::Seek(sample);
	}

	int FFmpegAudioDecoder::ReadCache(AudioBuffer<s16>& buffer, int count, int offset)
	{
		count = Math::Min((size_t)count, cacheLength / sizeof(AudioBuffer<s16>::AudioSample) / channels);
		buffer.DeinterlaceFrom((AudioBuffer<s16>::AudioSample*)cache, count, offset);
		cacheLength -= sizeof(AudioBuffer<s16>::AudioSample) * channels * count;
		memmove(cache, (u8*)cache + sizeof(AudioBuffer<s16>::AudioSample) * channels * count, cacheLength);
		return count;
	}

	u32 FFmpegAudioDecoder::Read(AudioBuffer<s16>& buffer, u32 count, u32 boffset)
	{
		int offset = ReadCache(buffer, count, boffset);
		count -= offset;
		while (count > 0) {
			AVPacket originalPacket;
			if (!decoder->ReadPacket(&originalPacket, streamIndex))
				break;

			AVPacket packet = originalPacket;

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

				int read = Math::Min((u32)datasize, count * 2 * channels);
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

			decoder->FreePacket(&originalPacket);
		}
		AudioCodec<s16>::Read(buffer, offset);
		return offset;
	}
} }
#endif
