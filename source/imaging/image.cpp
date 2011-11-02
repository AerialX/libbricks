#include "bricksall.hpp"

using namespace Bricks::IO;

namespace Bricks { namespace Imaging {
	void Image::CopyTo(const Pointer<Image>& image, u32 x, u32 y) const
	{
		CopyTo(image, x, y, GetWidth(), GetHeight());
	}

	void Image::CopyTo(const Pointer<Image>& image, u32 x, u32 y, u32 width, u32 height) const
	{
		if (x > image->GetWidth() || y > image->GetHeight())
			return;
		width = BRICKS_FEATURE_MIN(width, image->GetWidth() - x);
		height = BRICKS_FEATURE_MIN(height, image->GetHeight() - y);
		width = BRICKS_FEATURE_MIN(width, GetWidth());
		height = BRICKS_FEATURE_MIN(height, GetHeight());

		for (u32 i = x; i < x + width; i++) {
			for (u32 j = y; j < y + height; j++)
				image->SetPixel(i, j, GetPixel(i - x, j - y));
		}
	}

	void Image::ColouriseTo(const Pointer<Image>& image, const Colour& colour) const
	{
		if (image->GetWidth() != GetWidth() || image->GetHeight() != GetHeight())
			throw InvalidArgumentException();

		for (u32 x = 0; x < image->GetWidth(); x++)
			for (u32 y = 0; y < image->GetHeight(); y++)
				image->SetPixel(x, y, colour * GetPixel(x, y));
	}
} }
