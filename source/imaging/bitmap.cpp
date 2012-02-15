#include "bricks/imaging/bitmap.h"
#include "bricks/io/endian.h"

using namespace Bricks::IO;

namespace Bricks { namespace Imaging {
	Bitmap::Bitmap(u32 width, u32 height, const PixelDescription& description, InterlaceType::Enum interlacing) :
		pixelDescription(description), interlacing(interlacing), width(width), height(height), pixelData(NULL)
	{
		AllocateMemory();
	}

	Bitmap::~Bitmap()
	{
		if (pixelData)
			delete[] pixelData;
	}

	void Bitmap::AllocateMemory()
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

	Colour Bitmap::GetPixel(u32 x, u32 y) const
	{
		if (x >= width || y >= height)
			BRICKS_FEATURE_THROW(InvalidArgumentException());

		u64 pixel = 0;
		size_t offset = (size_t)y * width + x;
		switch (pixelDescription.GetPixelDepth()) {
			case 8:
				pixel = ((u8*)pixelData)[offset];
				break;
			case 16:
				pixel = EndianConvertLE16((u16*)pixelData + offset);
				break;
			case 32:
				pixel = EndianConvertLE32((u32*)pixelData + offset);
				break;
			case 64:
				pixel = EndianConvertLE64((u64*)pixelData + offset);
				break;
			default:
				offset *= pixelDescription.GetPixelDepth();
				for (int i = BRICKS_FEATURE_ROUND_UP(pixelDescription.GetPixelDepth(), 8) / 8 - 1; i >= 0; i--) {
					pixel <<= 8;
					pixel |= ((u8*)pixelData)[offset / 8 + i];
				}
				pixel >>= offset % 8;
				break;
		}

		Colour colour;

		for (int i = 0; i < ColourType::Count; i++) {
			u8 bitDepth = pixelDescription.GetBitDepth((ColourType::Enum)i);
			u32 component = pixel << (0x20 - bitDepth);
			component >>= (0x20 - bitDepth);
			pixel >>= bitDepth;
			if (bitDepth) {
				switch ((ColourType::Enum)i) {
					case ColourType::Intensity:
						// TODO: Chroma map
						colour.A = colour.R = colour.G = colour.B = component;
						break;
					case ColourType::Palette:
						colour = palette->GetItem(component);
						break;
					case ColourType::Red:
						colour.R = component;
						break;
					case ColourType::Green:
						colour.G = component;
						break;
					case ColourType::Blue:
						colour.B = component;
						break;
					case ColourType::Alpha:
						colour.A = component;
						break;
					default:
						break;
				}
			}
		}

		return colour;
	}

	void Bitmap::SetPixel(u32 x, u32 y, const Colour& colour)
	{
		if (x >= width || y >= height)
			BRICKS_FEATURE_THROW(InvalidArgumentException());
		
		u64 pixel = 0;

		for (int i = 0; i < ColourType::Count; i++) {
			u8 bitDepth = pixelDescription.GetBitDepth((ColourType::Enum)i);
			u32 component = 0;
			if (bitDepth) {
				switch ((ColourType::Enum)i) {
					case ColourType::Intensity:
						// TODO: Chroma map
						component = (colour.R + colour.G + colour.B) / 3;
						break;
					case ColourType::Palette:
						component = palette->IndexOfItem(colour);
						break;
					case ColourType::Red:
						component = colour.R;
						break;
					case ColourType::Green:
						component = colour.G;
						break;
					case ColourType::Blue:
						component = colour.B;
						break;
					case ColourType::Alpha:
						component = colour.A;
						break;
					default:
						break;
				}
			}
			component = component << (0x20 - bitDepth);
			component >>= (0x20 - bitDepth);
			pixel <<= bitDepth;
			pixel |= component;
		}

		size_t offset = (size_t)y * width + x;
		switch (pixelDescription.GetPixelDepth()) {
			case 8:
				((u8*)pixelData)[offset] = pixel;
				break;
			case 16:
				EndianConvertBE16((u16*)pixelData + offset, pixel);
				break;
			case 32:
				EndianConvertBE32((u32*)pixelData + offset, pixel);
				break;
			case 64:
				EndianConvertBE64((u64*)pixelData + offset, pixel);
				break;
			default:
				if (pixelDescription.GetPixelDepth() < 8)
					BRICKS_FEATURE_THROW(NotSupportedException());
				// TODO: This won't work for <8 bit depths because overlapping pixels need to be OR'd
				offset *= pixelDescription.GetPixelDepth();
				offset /= 8;
				for (int i = pixelDescription.GetPixelDepth() / 8 - 1; i >= 0; i--) {
					((u8*)pixelData)[offset + i] = (u8)pixel; 
					pixel >>= 8;
				}
				break;
		}
	}

	void Bitmap::CopyTo(Image* image, u32 x, u32 y, u32 width, u32 height) const
	{
		Bitmap* bitmap = CastToDynamic<Bitmap>(image);
		bool compatible = bitmap;
		const PixelDescription& description = GetPixelDescription();
		if (compatible) {
			bitmap = CastTo<Bitmap>(image);
			compatible = bitmap->GetPixelDescription() == description;
		}

		if (!compatible) {
			Image::CopyTo(image, x, y, width, height);
			return;
		}

		const u8* idata = (const u8*)GetImageData();
		u8* odata = (u8*)bitmap->GetImageData();
		u32 pixel = description.GetPixelSize();
		u32 stride = width * pixel;
		u32 istride = GetWidth() * pixel;
		u32 ostride = image->GetWidth() * pixel;
		u32 oindex = ostride * y + x * pixel;
		u32 index = 0;
		for (u32 i = y; i < height; i++, index += istride, oindex += ostride)
			memcpy(odata + oindex, idata + index, stride);
	}
} }
