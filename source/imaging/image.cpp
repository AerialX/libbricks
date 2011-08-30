#include "bricksall.hpp"

namespace Bricks { namespace Imaging {
	Colour Image::GetPixel(u32 x, u32 y)
	{
		if (x >= width || y >= height)
			throw InvalidArgumentException();

		u64 pixel = 0;
		size_t offset = (size_t)y * width + x;
		switch (pixelDescription.GetPixelDepth()) {
			case 8:
				pixel = ((u8*)pixelData)[offset];
				break;
			case 16:
				pixel = ((u16*)pixelData)[offset];
				break;
			case 32:
				pixel = ((u32*)pixelData)[offset];
				break;
			case 64:
				pixel = ((u64*)pixelData)[offset];
				break;
			default:
				offset *= pixelDescription.GetPixelDepth();
				for (int i = 0; i < ColourType::Count; i++) {
					u8 bitDepth = pixelDescription.GetBitDepth((ColourType::Enum)i);

					for (int k = 0; k < bitDepth; k++, offset++) {
						size_t byteOffset = offset / 8;
						u8 bitOffset = offset % 8;
						u8 data = ((u8*)pixelData)[byteOffset];
						data &= 0x80 >> bitOffset;
						data >>= 7 - bitOffset;
						pixel |= data << (bitDepth - k - 1);
					}
				}
				break;
		}

		Colour colour;

		for (ColourType::Enum i = ColourType::Unknown; i < ColourType::Count; i++) {
			u8 bitDepth = pixelDescription.GetBitDepth(i);
			u32 component = pixel << (0x20 - bitDepth);
			component >>= (0x20 - bitDepth);
			pixel >>= bitDepth;
			if (bitDepth) {
				switch (i) {
					case ColourType::Intensity:
						// TODO: Chroma map
						colour.R = colour.G = colour.B = component;
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

	void Image::SetPixel(u32 x, u32 y, const Colour& colour)
	{
		if (x >= width || y >= height)
			throw InvalidArgumentException();
		
	}
} }
