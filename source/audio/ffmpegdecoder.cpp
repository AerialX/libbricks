#include "bricks/audio/ffmpegdecoder.h"
#include "bricks/core/exception.h"
#include "bricks/io/stream.h"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
}

#if LIBAVFORMAT_VERSION_MAJOR <= 53 && LIBAVFORMAT_VERSION_MINOR < 18
#define BRICKS_FEATURE_FFMPEG_OLD
#endif

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Audio;

static bool ffmpegInitialized = false;
static void FFmpegInit()
{
	if (!ffmpegInitialized) {
		av_register_all();
		ffmpegInitialized = true;
	}
}

static int FFmpegRead(void* userData, uint8_t* buffer, int bufferSize)
{
	return CastToRaw<Stream>(userData)->Read(buffer, bufferSize);
}

static int FFmpegWrite(void* userData, uint8_t* buffer, int bufferSize)
{
	return CastToRaw<Stream>(userData)->Write(buffer, bufferSize);
}

static int64_t FFmpegSeek(void* userData, int64_t offset, int whence)
{
	whence &= ~AVSEEK_FORCE;
	Stream* stream = CastToRaw<Stream>(userData);
	s64 position;
	switch (whence) {
		case SEEK_SET:
			position = offset;
			break;
		case SEEK_CUR:
			position = stream->GetPosition() + offset;
			break;
		case SEEK_END:
			position = stream->GetLength() + offset;
			break;
		case AVSEEK_SIZE:
			return stream->GetLength();
		default:
			BRICKS_FEATURE_THROW(InvalidArgumentException());
	}
	stream->SetPosition(position);
	return stream->GetPosition();
}

FFmpegDecoder::FFmpegDecoder(Stream* ioStream) : ioStream(ioStream)
{
	FFmpegInit();

	format = avformat_alloc_context();
	format->flags |= AVFMT_FLAG_CUSTOM_IO;
	ffmpegBuffer = av_malloc(ffmpegBufferSize);
	AVIOContext* ffmpegStream = avio_alloc_context((unsigned char*)ffmpegBuffer, ffmpegBufferSize, 0, CastToRaw(ioStream), &FFmpegRead, &FFmpegWrite, &FFmpegSeek);
	ffmpegStream->seekable = ioStream->CanSeek() ? AVIO_SEEKABLE_NORMAL : 0;
	format->pb = ffmpegStream;
	if (avformat_open_input(&format, "", NULL, NULL))
		BRICKS_FEATURE_THROW(Exception());
#ifdef BRICKS_FEATURE_FFMPEG_OLD
	if (av_find_stream_info(format))
#else
	if (avformat_find_stream_info(format, NULL))
#endif
		BRICKS_FEATURE_THROW(FormatException());

	ResetFrameTime();
}

FFmpegDecoder::~FFmpegDecoder()
{
	foreach (int streamIndex, openStreams)
		CloseStream(streamIndex);

	AVIOContext* ffmpegStream = format->pb;
#ifdef BRICKS_FEATURE_FFMPEG_OLD
	av_close_input_file(format);
#else
	avformat_close_input(&format);
#endif
	av_free(ffmpegStream);
}

AVCodec* FFmpegDecoder::OpenStream(int streamIndex)
{
	if (streamIndex < 0 || streamIndex >= (int)format->nb_streams || !format->streams[streamIndex])
		BRICKS_FEATURE_THROW(InvalidArgumentException());

	AVStream* stream = format->streams[streamIndex];
	AVCodec* codec = avcodec_find_decoder(stream->codec->codec_id);

	if (
#ifdef BRICKS_FEATURE_FFMPEG_OLD
		avcodec_open(stream->codec, codec)
#else
		avcodec_open2(stream->codec, codec, NULL)
#endif
	)
		BRICKS_FEATURE_THROW(FormatException());

	openStreams.AddItem(streamIndex);

	return codec;
}

void FFmpegDecoder::CloseStream(int streamIndex)
{
	if (!IsStreamOpen(streamIndex))
		return;

	AVStream* stream = format->streams[streamIndex];
	avcodec_close(stream->codec);

	openStreams.RemoveItem(streamIndex);
}

bool FFmpegDecoder::IsStreamOpen(int streamIndex)
{
	return openStreams.ContainsItem(streamIndex);
}

bool FFmpegDecoder::ReadPacket(AVPacket* packet, int streamIndex)
{
	if (DequeuePacket(packet, streamIndex))
		return true;

	while (!av_read_frame(format, packet)) {
		currentTimeKnown = true;
		currentTime = packet->dts;

		if (streamIndex < 0 || packet->stream_index == streamIndex)
			return true;
		else if (!IsStreamOpen(packet->stream_index))
			continue;
		else
			QueuePacket(packet);
	}

	return false;
}

void FFmpegDecoder::FreePacket(AVPacket* packet)
{
	av_free_packet(packet);
}

void FFmpegDecoder::QueuePacket(AVPacket* packet)
{
	av_dup_packet(packet);
	packetQueue.AddItem(*packet);
}

bool FFmpegDecoder::DequeuePacket(AVPacket* packet, int streamIndex)
{
	foreach (const AVPacket& queuedPacket, packetQueue) {
		if (streamIndex < 0 || queuedPacket.stream_index == streamIndex) {
			*packet = queuedPacket;
			packetQueue.RemoveItem(queuedPacket);
			return true;
		}
	}

	return false;
}

void FFmpegDecoder::FlushQueue()
{
	foreach (AVPacket& packet, packetQueue)
		av_free_packet(&packet);
	packetQueue.Clear();
}

void FFmpegDecoder::SeekFrame(s64 time)
{
	av_seek_frame(format, 0, time, (time < GetFrameTime() ? AVSEEK_FLAG_BACKWARD :0));
	foreach (int streamIndex, openStreams)
		avcodec_flush_buffers(format->streams[streamIndex]->codec);
	FlushQueue();
	ResetFrameTime();
}

s64 FFmpegDecoder::GetFrameTime()
{
	if (!currentTimeKnown) {
		if (packetQueue.GetCount()) {
			currentTimeKnown = true;
			currentTime = packetQueue.GetItem(0).dts;
		} else {
			AVPacket packet;
			if (!ReadPacket(&packet))
				return format->duration; // Assume end of stream
			QueuePacket(&packet);
		}
	}

	return currentTime;
}

void FFmpegDecoder::ResetFrameTime()
{
	currentTimeKnown = false;
}
