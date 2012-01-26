#pragma once

#include "bricks/imaging/image.h"
#include "bricks/imaging/bitmap.h"

namespace Bricks { namespace Imaging {
	class Subimage : public BitmapImage
	{
	protected:
		AutoPointer<Image> image;
		BitmapImage* bitmap;
		u32 offsetX;
		u32 offsetY;
		u32 width;
		u32 height;

	public:
		Subimage(BitmapImage* image, u32 offsetX = 0, u32 offsetY = 0, u32 width = 0, u32 height = 0) : image(image), bitmap(image), offsetX(offsetX), offsetY(offsetY), width(width), height(height) { if (bitmap && bitmap->GetInterlaceType() != InterlaceType::None) BRICKS_FEATURE_THROW(NotSupportedException()); }
		Subimage(Image* image, u32 offsetX = 0, u32 offsetY = 0, u32 width = 0, u32 height = 0) : image(image), bitmap(CastToDynamic<BitmapImage>(image)), offsetX(offsetX), offsetY(offsetY), width(width), height(height) { if (bitmap && bitmap->GetInterlaceType() != InterlaceType::None) BRICKS_FEATURE_THROW(NotSupportedException()); }
		void* GetImageData() const { if (!bitmap) BRICKS_FEATURE_THROW(NotSupportedException()); return bitmap->GetImageData(); }
		u32 GetImageDataSize() const { if (!bitmap) BRICKS_FEATURE_THROW(NotSupportedException()); return Bitmap::CalculateImageDataSize(image->GetWidth(), height, bitmap->GetPixelDescription()); }
		u32 GetImageDataStride() const { if (!bitmap) BRICKS_FEATURE_THROW(NotSupportedException()); return bitmap->GetImageDataStride(); }
		const PixelDescription& GetPixelDescription() const { if (!bitmap) BRICKS_FEATURE_THROW(NotSupportedException()); return bitmap->GetPixelDescription(); }
		InterlaceType::Enum GetInterlaceType() const { return InterlaceType::None; }

		u32 GetOffsetX() const { return offsetX; }
		u32 GetOffsetY() const { return offsetY; }
		u32 GetWidth() const { return width; }
		u32 GetHeight() const { return height; }
		void SetOffset(u32 offsetX, u32 offsetY) { this->offsetX = offsetX; this->offsetY = offsetY; }
		void SetSize(u32 width, u32 height) { this->width = width; this->height = height; }

		Colour GetPixel(u32 x, u32 y) const { if (x >= width || y >= height) BRICKS_FEATURE_RELEASE_THROW_FATAL(InvalidArgumentException()); return image->GetPixel(offsetX + x, offsetY + y); }
		void SetPixel(u32 x, u32 y, const Colour& colour) { if (x >= width || y >= height) BRICKS_FEATURE_RELEASE_THROW_FATAL(InvalidArgumentException()); image->SetPixel(offsetX + x, offsetY + y, colour); }
	};
} }
