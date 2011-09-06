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
		u8* pixelData;

		static size_t CalculateImageDataSize(u32 width, u32 height, const PixelDescription& description) { return BRICKS_FEATURE_ROUND_UP((size_t)description.GetPixelDepth() * width * height, 8) / 8; }

		void AllocateMemory()
		{
			if (pixelData)
				delete[] pixelData;

			int size = GetImageDataSize();
			if (size) {
				pixelData = new u8[size];
				memset(pixelData, 0, size);
			} else
				pixelData = NULL;
		}

	public:
		// TODO: Should probably obey some packing rules
		Image(u32 width = 0, u32 height = 0, const PixelDescription& description = PixelDescription::RGBA8, InterlaceType::Enum interlacing = InterlaceType::None) : pixelDescription(description), interlacing(interlacing), width(width), height(height), pixelData(NULL) {
			AllocateMemory();
		}
		~Image() { if (pixelData) delete[] pixelData; }
		ReturnPointer<Palette> GetPalette() const { return palette; }
		void* GetImageData() const { return pixelData; }
		u32 GetImageDataSize() const { return CalculateImageDataSize(width, height, pixelDescription); }
		const PixelDescription& GetPixelDescription() const { return pixelDescription; }
		InterlaceType::Enum GetInterlaceType() const { return interlacing; }

		u32 GetWidth() const { return width; }
		u32 GetHeight() const { return height; }
		void SetSize(u32 width, u32 height) { this->width = width; this->height = height; AllocateMemory(); }

		Colour GetPixel(u32 x, u32 y);
		void SetPixel(u32 x, u32 y, const Colour& colour);

		void CopyTo(const Pointer<Image>& image, u32 x = 0, u32 y = 0);
		void CopyTo(const Pointer<Image>& image, u32 x, u32 y, u32 width, u32 height);
	};
} }
