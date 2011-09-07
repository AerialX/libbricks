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
				x = 0;
				y += GetHeight();
				continue;
			}
			AutoPointer<FontGlyph> glyph = GetGlyph(character);

			x += GetKerning(glyph, previous);

			x += glyph->GetAdvance();

			if (x > width)
				width = x;
			if (y + glyph->GetHeight() > height)
				height = y + glyph->GetHeight();
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
				return 0;
		}
	}

	ReturnPointer<Image> Font::DrawString(const String& value, FontAlignment::Enum alignment)
	{
		s32 x = 0;
		s32 width = 0;
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
				lineLengths.AddItem(x);
				x = 0;
				continue;
			}
			AutoPointer<FontGlyph> glyph = GetGlyph(character);

			x += GetKerning(glyph, previous);

			glyphs[i] = RenderedGlyph(glyph, lineLengths.GetCount(), x, lineLengths.GetCount() * GetHeight());

			x += glyph->GetAdvance();

			if (x > width)
				width = x;
		}
		lineLengths.AddItem(x);
		u32 line = 0;
		s32 offset = GetLineOffset(alignment, width, lineLengths[line]);
		AutoPointer<Image> image = autonew Image(width, lineLengths.GetCount() * GetHeight(), PixelDescription::I8);
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
