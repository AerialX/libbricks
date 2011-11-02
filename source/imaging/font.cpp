#include "bricksall.hpp"

using namespace Bricks::Collections;

namespace Bricks { namespace Imaging {
	struct RenderedGlyph {
		AutoPointer<FontGlyph> glyph;
		s32 line;
		s32 x;
		s32 y;
		RenderedGlyph(const Pointer<FontGlyph>& glyph = NULL, u32 line = 0, s32 x = 0, s32 y = 0) : glyph(glyph), line(line), x(x), y(y) { }
	};

	static AutoPointer<List<RenderedGlyph> > MeasureGlyphs(const Pointer<Font>& font, const String& value)
	{
		s32 x = 0;
		s32 y = font->GetBaseline();
		s32 line = 0;
		AutoPointer<FontGlyph> previous;
		AutoPointer<Array<RenderedGlyph> > glyphs = autonew Array<RenderedGlyph>();

		for (u32 i = 0; i < value.GetLength(); i++) {
			String::Character character = value[i];
			if (character == '\n') {
				previous = NULL;
				y += font->GetHeight();
				line++;
				x = 0;
				continue;
			}

			AutoPointer<FontGlyph> glyph = font->GetGlyph(character);
			if (!glyph)
				continue;

			x += font->GetKerning(glyph, previous);
			previous = glyph;

			glyphs->AddItem(RenderedGlyph(glyph, line, x, y));

			x += glyph->GetAdvance();
		}

		return glyphs;
	}

	static FontMeasureSize MeasureGlyphs(const Pointer<Font>& font, const Pointer<List<RenderedGlyph> >& glyphs)
	{
		s32 width = 0;
		s32 height = 0;
		foreach (const RenderedGlyph& glyph, glyphs) {
			s32 h1 = glyph.y - font->GetDescender();
			s32 h2 = glyph.y - glyph.glyph->GetBearingY() + glyph.glyph->GetHeight();
			s32 h = BRICKS_FEATURE_MAX(h1, h2);
			if (h > height)
				height = h;
			s32 w1 = glyph.x + glyph.glyph->GetAdvance();
			s32 w2 = glyph.x + glyph.glyph->GetBearingX() + glyph.glyph->GetWidth();
			s32 w = BRICKS_FEATURE_MAX(w1, w2);
			if (w > width)
				width = w;
		}
		return FontMeasureSize(width, height);
	}

	FontMeasureSize Font::MeasureString(const String& value)
	{
		return MeasureGlyphs(this, MeasureGlyphs(this, value));
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
		AutoPointer<List<RenderedGlyph> > glyphs = MeasureGlyphs(this, value);
		FontMeasureSize size = MeasureGlyphs(this, glyphs);

		AutoPointer<Bitmap> image = autonew Bitmap(size.GetWidth(), size.GetHeight(), PixelDescription::RGBA8);

		s32 line = 0;
		s32 width = 0;
		Array<s32> lineLengths;
		foreach (const RenderedGlyph& glyph, glyphs) {
			while (line < glyph.line) {
				lineLengths.AddItem(width);
				width = 0;
			}
			width = glyph.x + glyph.glyph->GetAdvance();
		}
		lineLengths.AddItem(width);

		s32 offset = 0;
		line = -1;
		foreach (const RenderedGlyph& glyph, glyphs) {
			while (line < glyph.line) {
				line++;
				offset = GetLineOffset(alignment, size.GetWidth(), lineLengths[line]);
			}

			glyph.glyph->GetImage()->CopyTo(image, offset + glyph.x + glyph.glyph->GetBearingX(), glyph.y - glyph.glyph->GetBearingY());
		}
		return image;
	}
} }
