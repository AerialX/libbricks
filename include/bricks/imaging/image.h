#pragma once

#include "bricks/object.h"
#include "bricks/imaging/colour.h"

namespace Bricks { namespace Imaging {
	namespace InterlaceType {
		enum Enum {
			None = 0,
			Adam7
		};
	}

	class Image : public Object, NoCopy
	{
	protected:
		AutoPointer<Palette> palette;
		PixelDescription pixelDescription;
		InterlaceType::Enum interlacing;
		u32 width;
		u32 height;
		void* pixelData;

	public:
		// TODO: Should probably obey some packing rules
		Image(u32 width = 0, u32 height = 0, const PixelDescription& description = PixelDescription::RGBA8, InterlaceType::Enum interlacing = InterlaceType::None) : pixelDescription(description), interlacing(interlacing), width(width), height(height), pixelData((width && height) ? new u8[BRICKS_FEATURE_ROUND_UP((size_t)pixelDescription.GetPixelDepth() * width * height, 8) / 8] : NULL) { if (pixelData) memset(pixelData, 0, GetImageDataSize()); }
		ReturnPointer<Palette> GetPalette() const { return palette; }
		void* GetImageData() const { return pixelData; }
		u32 GetImageDataSize() const { return BRICKS_FEATURE_ROUND_UP((size_t)pixelDescription.GetPixelDepth() * width * height, 8) / 8; }
		const PixelDescription& GetPixelDescription() const { return pixelDescription; }
		InterlaceType::Enum GetInterlaceType() const { return interlacing; }

		u32 GetWidth() const { return width; }
		u32 GetHeight() const { return height; }

		Colour GetPixel(u32 x, u32 y);
		void SetPixel(u32 x, u32 y, const Colour& colour);
	};
} }
