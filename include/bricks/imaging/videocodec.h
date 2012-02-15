#pragma once

#include "bricks/imaging/image.h"

namespace Bricks { namespace Imaging {
	class VideoCodec : public Object
	{
	protected:
		float framesPerSecond;
		s64 frameCount;
		s32 frameWidth;
		s32 frameHeight;
		PixelDescription pixelDescription;
		s64 position;

	public:
		VideoCodec();

		float GetFrameRate() const { return framesPerSecond; }
		s64 GetFrameCount() const { return frameCount; }
		s32 GetFrameWidth() const { return frameWidth; }
		s32 GetFrameHeight() const { return frameHeight; }
		const PixelDescription& GetPixelDescription() const { return pixelDescription; }

		void SetPixelDescription(const PixelDescription& value) { pixelDescription = value; }

		s64 GetPosition() { return position; }

		void Seek(s64 frame);
		ReturnPointer<BitmapImage> Read();

		virtual bool Read(BitmapImage* image) = 0;
	};
} }
