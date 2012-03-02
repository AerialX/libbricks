#include "bricks/imaging/videocodec.h"
#include "bricks/imaging/bitmap.h"

namespace Bricks { namespace Imaging {
	VideoCodec::VideoCodec() :
		framesPerSecond(0), frameCount(0), frameWidth(0), frameHeight(0), position(0)
	{

	}

	void VideoCodec::Seek(s64 frame)
	{
		position = frame;
	}

	ReturnPointer<BitmapImage> VideoCodec::Read(s64 frame)
	{
		AutoPointer<Bitmap> bitmap = autonew Bitmap(frameWidth, frameHeight, pixelDescription);
		if (!Read(bitmap, frame))
			return NULL;
		return bitmap;
	}
} }
