#include "bricks/imaging/ffmpegvideodecoder.h"
#include "bricks/imaging/image.h"
#include "bricks/core/exception.h"

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Imaging;

static PixelDescription FFmpegPixelFormat(PixelFormat format)
{
	switch (format) {
		case PIX_FMT_GRAY8:
			return PixelDescription::I8;
		case PIX_FMT_GRAY8A:
			return PixelDescription::IA8;
		case PIX_FMT_MONOWHITE:
		case PIX_FMT_MONOBLACK:
			return PixelDescription(1, ColourTypeMask::Intensity);
		case PIX_FMT_RGBA:
			return PixelDescription::RGBA8;
		case PIX_FMT_GRAY16BE:
		case PIX_FMT_GRAY16LE:
			return PixelDescription(16, ColourTypeMask::Intensity);
		case PIX_FMT_0RGB:
			return PixelDescription(8, ColourTypeMask::Unknown | ColourTypeMask::RGB);
		case PIX_FMT_RGB24:
		default:
			return PixelDescription::RGB8;
	}
}

static PixelFormat FFmpegPixelFormat(const PixelDescription& description)
{
	return PIX_FMT_RGB24;
}

FFmpegVideoDecoder::FFmpegVideoDecoder(Audio::FFmpegDecoder* decoder) :
	decoder(decoder)
{
	Initialize();
}

FFmpegVideoDecoder::FFmpegVideoDecoder(Stream* ioStream) :
	decoder(autonew Audio::FFmpegDecoder(ioStream))
{
	Initialize();
}

void FFmpegVideoDecoder::Initialize()
{
	AVFormatContext* format = decoder->GetFormatContext();
	for (streamIndex = 0; streamIndex < format->nb_streams; streamIndex++) {
		stream = format->streams[streamIndex];
		if (!stream)
			continue;
		if (stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			break;
	}

	if (streamIndex >= format->nb_streams)
		BRICKS_FEATURE_THROW(FormatException());

	frame = avcodec_alloc_frame();
	packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	originalPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
	av_init_packet(packet);
	av_init_packet(originalPacket);
	packet->data = originalPacket->data = NULL;
	packet->size = originalPacket->size = 0;
	packet->dts = originalPacket->dts = -1;

	codec = decoder->OpenStream(streamIndex);

	if (stream->codec->time_base.num && stream->codec->time_base.den && stream->codec->ticks_per_frame)
		framesPerSecond = (float)stream->codec->time_base.den / stream->codec->time_base.num / stream->codec->ticks_per_frame;
	else if (stream->r_frame_rate.num && stream->r_frame_rate.den)
		framesPerSecond = (float)stream->r_frame_rate.num / stream->r_frame_rate.den;

	frameWidth = stream->codec->coded_width;
	frameHeight = stream->codec->coded_height;

	frameCount = stream->duration;

	pixelDescription = FFmpegPixelFormat(stream->codec->pix_fmt);
	
	swsContext = sws_getContext(frameWidth, frameHeight, stream->codec->pix_fmt, frameWidth, frameHeight, PIX_FMT_RGB24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	swsFrame = avcodec_alloc_frame();
}

FFmpegVideoDecoder::~FFmpegVideoDecoder()
{
	decoder->FreePacket(originalPacket);
	av_free(originalPacket);
	av_free(packet);
	av_free(frame);
}

void FFmpegVideoDecoder::Seek(s64 frame)
{
	decoder->SeekFrame(av_rescale(frame, stream->time_base.den, stream->time_base.num));
	VideoCodec::Seek(frame);
}

bool FFmpegVideoDecoder::Read(BitmapImage* image, s64 targetFrame)
{
	while (true) {
		while (packet->size <= 0) {
			decoder->FreePacket(originalPacket);
			if (!decoder->ReadPacket(originalPacket, streamIndex))
				return false;
			*packet = *originalPacket;
		}

		int success;
		int used = avcodec_decode_video2(stream->codec, frame, &success, packet);
		if (used < 0)
			break;

		packet->size -= used;
		packet->data += used;

		if (success && packet->dts >= targetFrame) {
			if (frame->repeat_pict)
				BRICKS_FEATURE_THROW(NotSupportedException());
			bool customContext = false;
			SwsContext* convertContext = swsContext;
			PixelFormat pixelFormat = FFmpegPixelFormat(image->GetPixelDescription());
			if ((int)image->GetWidth() != frameWidth || (int)image->GetHeight() != frameHeight || pixelFormat != PIX_FMT_RGB24) {
				convertContext = sws_getContext(frameWidth, frameHeight, stream->codec->pix_fmt, image->GetWidth(), image->GetHeight(), pixelFormat, SWS_FAST_BILINEAR, NULL, NULL, NULL);
				customContext = true;
			}
			
			avpicture_fill((AVPicture*)swsFrame, (u8*)image->GetImageData(), pixelFormat, image->GetWidth(), image->GetHeight());
			sws_scale(convertContext, frame->data, frame->linesize, 0, frameHeight, swsFrame->data, swsFrame->linesize);
			if (customContext)
				av_free(convertContext);
			
			position = packet->dts;
			return true;
		}
	}

	return false;
}
