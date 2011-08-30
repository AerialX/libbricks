#include "bricksall.hpp"

namespace Bricks { namespace Imaging {
	const ColourTypeMask::Enum PixelDescription::bitDepthMap[] = {
		ColourTypeMask::Unknown,
		ColourTypeMask::Intensity,
		ColourTypeMask::Palette,
		ColourTypeMask::Red,
		ColourTypeMask::Green,
		ColourTypeMask::Blue,
		ColourTypeMask::Alpha
	};
	const PixelDescription PixelDescription::RGB8 = PixelDescription(8, ColourTypeMask::RGB);
	const PixelDescription PixelDescription::RGBA8 = PixelDescription(8, ColourTypeMask::RGBA);
} }
