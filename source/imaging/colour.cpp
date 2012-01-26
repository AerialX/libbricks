#include "bricks/imaging/colour.h"

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
	const PixelDescription PixelDescription::I8 = PixelDescription(8, ColourTypeMask::Intensity);
	const PixelDescription PixelDescription::IA8 = PixelDescription(8, ColourTypeMask::IntensityAlpha);
	const PixelDescription PixelDescription::RGB8 = PixelDescription(8, ColourTypeMask::RGB);
	const PixelDescription PixelDescription::RGBA8 = PixelDescription(8, ColourTypeMask::RGBA);
} }
