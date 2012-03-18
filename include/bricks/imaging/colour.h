#pragma once

#include "bricks/core/types.h"
#include "bricks/core/math.h"
#include "bricks/collections/list.h"

namespace Bricks { namespace Imaging {
	class Colour
	{
	public:
		Colour(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 0xFF) : R(r), G(g), B(b), A(a) { }
		bool operator==(const Colour& rhs) const { return R == rhs.R && G == rhs.G && B == rhs.B && A == rhs.A; }
		bool operator!=(const Colour& rhs) const { return !operator==(rhs); }

		u8 R;
		u8 G;
		u8 B;
		u8 A;

		Colour operator+(const Colour& rhs) const { return Colour(R + rhs.R, G + rhs.G, B + rhs.B, A + rhs.A); }
		Colour operator-(const Colour& rhs) const { return Colour(R - rhs.R, G - rhs.G, B - rhs.B, A - rhs.A); }
		Colour operator*(const Colour& rhs) const { return Colour((int)R * rhs.R / 0xFF, (int)G * rhs.G / 0xFF, (int)B * rhs.B / 0xFF, (int)A * rhs.A / 0xFF); }
		Colour operator/(const Colour& rhs) const { return Colour((int)R * 0xFF / rhs.R, (int)G * 0xFF / rhs.G, (int)B * 0xFF / rhs.B, (int)A * 0xFF / rhs.A); }

		Colour& operator+=(const Colour& rhs) { *this = *this + rhs; return *this; }
		Colour& operator-=(const Colour& rhs) { *this = *this / rhs; return *this; }
		Colour& operator*=(const Colour& rhs) { *this = *this * rhs; return *this; }
		Colour& operator/=(const Colour& rhs) { *this = *this / rhs; return *this; }
	};

	typedef Collections::List<Colour> Palette;

	namespace ColourType {
		enum Enum {
			Unknown = 0,
			Intensity,
			Palette,
			Red,
			Green,
			Blue,
			Alpha,
			Count
		};
	}

	namespace ColourTypeMask {
		enum Enum {
			Unknown			= 0x00,
			Intensity		= 0x01,
			Palette			= 0x02,
			Red				= 0x04,
			Green			= 0x08,
			Blue			= 0x10,
			Alpha			= 0x20,

			IntensityAlpha	= Intensity | Alpha,
			RGB				= Red | Green | Blue,
			RGBA			= RGB | Alpha,
		};
	}

	// TODO: Information on pixel component order (like BGRA)
	class PixelDescription
	{
	protected:
		u8 bitDepth[(int)ColourType::Count];
		static const ColourTypeMask::Enum bitDepthMap[];

	public:
		PixelDescription() { memset(bitDepth, 0, sizeof(bitDepth)); }
		PixelDescription(u8 bitDepth, int mask) { for (int i = 0; i < ColourType::Count; i++) this->bitDepth[i] = (mask & bitDepthMap[i]) ? bitDepth : 0; }
		ColourTypeMask::Enum GetBitMask() const { int mask = 0; for (int i = 0; i < ColourType::Count; i++) { if (bitDepth[i]) mask |= bitDepthMap[i]; } return (ColourTypeMask::Enum)mask; }
		u8 GetBitDepth(ColourType::Enum type) const { return bitDepth[(int)type]; }
		u8 GetColourCount() const { u8 ret = 0; for (int i = 0; i < ColourType::Count; i++) { if (bitDepth[i]) ret++; } return ret; }
		u8 GetPixelDepth() const { u8 ret = 0; for (int i = 0; i < ColourType::Count; i++) ret += bitDepth[i]; return ret; }
		u8 GetPixelSize() const { return Math::RoundUp(GetPixelDepth(), 8) / 8; }

		bool operator==(const PixelDescription& description) const { return GetBitMask() == description.GetBitMask(); }
		bool operator!=(const PixelDescription& description) const { return !operator==(description); }

		static const PixelDescription I8;
		static const PixelDescription IA8;
		static const PixelDescription RGB8;
		static const PixelDescription RGBA8;
	};
} }
