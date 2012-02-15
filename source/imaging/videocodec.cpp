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

	ReturnPointer<BitmapImage> VideoCodec::Read()
	{
		AutoPointer<Bitmap> bitmap = autonew Bitmap(frameWidth, frameHeight, pixelDescription);
		if (!Read(bitmap))
			return NULL;
		return bitmap;
	}
} }
