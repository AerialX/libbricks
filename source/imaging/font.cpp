#include "bricksall.hpp"

using namespace Bricks::Collections;

namespace Bricks { namespace Imaging {
	FontMeasureSize Font::MeasureString(const String& value)
	{
		s32 width = 0;
		s32 height = 0;
		s32 x = 0;
		s32 y = 0;
		AutoPointer<FontGlyph> previous;
		for (u32 i = 0; i < value.GetLength(); i++) {
			String::Character character = value[i];
			if (character == '\n') {
				if (previous->GetWidth() > previous->GetAdvance()) {
					x += previous->GetWidth() - previous->GetAdvance();
					if (x > width)
						width = x;
				}
				x = 0;
				y += GetHeight();
				previous = NULL;
				continue;
			}
			AutoPointer<FontGlyph> glyph = GetGlyph(character);
			if (!glyph)
				continue;

			x += GetKerning(glyph, previous);
			previous = glyph;

			s32 gwidth = x + glyph->GetWidth() + glyph->GetBearingX();
			if (gwidth > width)
				width = gwidth;

			x += glyph->GetAdvance();

			if (x > width)
				width = x;
			if (y + GetHeight() > (u32)height)
				height = y + GetHeight();
			s32 gheight = glyph->GetHeight() + GetHeight() - glyph->GetBearingY();
			if (y + gheight > height)
				height = y + gheight;
		}
		return FontMeasureSize(width, height);
	}

	static s32 GetLineOffset(FontAlignment::Enum alignment, s32 width, s32 lineLength)
	{
		switch (alignment) {
			case FontAlignment::Centre:
				return (width - lineLength) / 2;
			case FontAlignment::Right:
				return width - lineLength;
			case FontAlignment::Left:
			default:
				return 0;
		}
	}

	ReturnPointer<Image> Font::DrawString(const String& value, FontAlignment::Enum alignment)
	{
		s32 x = 0;
		s32 width = 0;
		s32 height = 0;
		struct RenderedGlyph {
			AutoPointer<FontGlyph> glyph;
			u32 line;
			s32 x;
			s32 y;
			RenderedGlyph(const Pointer<FontGlyph>& glyph = NULL, u32 line = 0, s32 x = 0, s32 y = 0) : glyph(glyph), line(line), x(x), y(y) { }
		};
		Array<s32> lineLengths;
		RenderedGlyph* glyphs = new RenderedGlyph[value.GetLength()];
		AutoPointer<FontGlyph> previous;
		for (u32 i = 0; i < value.GetLength(); i++) {
			String::Character character = value[i];
			if (character == '\n') {
				if (previous->GetWidth() > previous->GetAdvance()) {
					x += previous->GetWidth() - previous->GetAdvance();
					if (x > width)
						width = x;
				}
				lineLengths.AddItem(x);
				x = 0;
				previous = NULL;
				continue;
			}
			AutoPointer<FontGlyph> glyph = GetGlyph(character);
			if (!glyph)
				continue;

			x += GetKerning(glyph, previous);
			previous = glyph;

			glyphs[i] = RenderedGlyph(glyph, lineLengths.GetCount(), x, lineLengths.GetCount() * GetHeight());

			s32 gwidth = x + glyph->GetWidth() + glyph->GetBearingX();
			if (gwidth > width)
				width = gwidth;

			x += glyph->GetAdvance();

			if (x > width)
				width = x;
			s32 y = lineLengths.GetCount() * GetHeight();
			if (y + GetHeight() > (u32)height)
				height = y + GetHeight();
			s32 gheight = glyph->GetHeight() + GetHeight() - glyph->GetBearingY();
			if (y + gheight > height)
				height = y + gheight;
		}
		lineLengths.AddItem(x);
		u32 line = 0;
		s32 offset = GetLineOffset(alignment, width, lineLengths[line]);
		AutoPointer<Bitmap> image = autonew Bitmap(width, height, PixelDescription::RGBA8);
		for (u32 i = 0; i < value.GetLength(); i++) {
			if (value[i] == '\n') {
				line++;
				offset = GetLineOffset(alignment, width, lineLengths[line]);
			}

			Pointer<FontGlyph> glyph = glyphs[i].glyph;
			if (!glyph)
				continue;

			glyph->GetImage()->CopyTo(image, offset + glyphs[i].x + glyph->GetBearingX(), glyphs[i].y + GetHeight() - glyph->GetBearingY());
		}
		delete[] glyphs;
		return image;
	}
} }
