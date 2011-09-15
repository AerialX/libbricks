#pragma once

#include "bricks/object.h"
#include "bricks/imaging/image.h"
#include "bricks/collections/dictionary.h"

namespace Bricks { namespace Imaging {
	class FontGlyph;

	namespace FontAlignment {
		enum Enum {
			Left = 0,
			Centre,
			Right
		};
	};

	struct FontMeasureSize
	{
	protected:
		u32 width;
		u32 height;
	public:
		FontMeasureSize(u32 width = 0, u32 height = 0) : width(width), height(height) { }

		u32 GetWidth() const { return width; }
		u32 GetHeight() const { return height; }
	};

	class Font : public Object
	{
	protected:
		Collections::Dictionary<String::Character, AutoPointer<FontGlyph> > glyphCache;
		u32 width;
		u32 height;

		virtual ReturnPointer<FontGlyph> LoadGlyph(String::Character character) = 0;
		virtual ReturnPointer<Image> RenderGlyph(const Pointer<FontGlyph>& glyph) = 0;

		friend class FontGlyph;

	public:
		Font() : width(0), height(0) { }

		virtual void SetPixelSize(int pixelWidth, int pixelHeight = 0) { width = pixelWidth; height = pixelHeight ?: pixelWidth; }
		virtual void SetPointSize(int pointWidth, int pointHeight = 0, int dpiWidth = 0, int dpiHeight = 0) { width = pointWidth; height = pointHeight ?: pointWidth; }

		u32 GetWidth() const { return width; }
		u32 GetHeight() const { return height; }

		FontMeasureSize MeasureString(const String& value);

		ReturnPointer<Image> DrawString(const String& value, FontAlignment::Enum alignment = FontAlignment::Left);

		void ClearCache() { glyphCache.Clear(); }
		ReturnPointer<FontGlyph> GetGlyph(String::Character character, bool cache = true) { if (glyphCache.ContainsKey(character)) return glyphCache[character]; AutoPointer<FontGlyph> glyph = LoadGlyph(character); if (cache) glyphCache.Add(character, glyph); return glyph; }

		virtual s32 GetKerning(const Pointer<FontGlyph>& glyph, const Pointer<FontGlyph>& previous) { return 0; }
	};

	class FontGlyph : public Object
	{
	protected:
		Pointer<Font> font;
		AutoPointer<Image> image;
		String::Character character;
		int index;
		s32 advance;
		s32 width;
		s32 height;
		s32 bearingX;
		s32 bearingY;

	public:
		FontGlyph(const Pointer<Font>& font, String::Character character, int index, s32 advance, s32 width, s32 height, s32 bearingX, s32 bearingY) : font(font), character(character), index(index), advance(advance), width(width), height(height), bearingX(bearingX), bearingY(bearingY) { }
		FontGlyph(const Pointer<Font>& font, String::Character character, const Pointer<Image>& image, int index, s32 advance, s32 width, s32 height, s32 bearingX, s32 bearingY) : font(font), image(image), character(character), index(index), advance(advance), width(width), height(height), bearingX(bearingX), bearingY(bearingY) { }

		ReturnPointer<Font> GetFont() const { return font; }
		ReturnPointer<Image> GetImage() { if (image) return image; image = font->RenderGlyph(this); return image; }
		String::Character GetCharacter() const { return character; }
		int GetIndex() const { return index; }
		s32 GetAdvance() const { return advance; }
		s32 GetWidth() const { return width; }
		s32 GetHeight() const { return height; }
		s32 GetBearingX() const { return bearingX; }
		s32 GetBearingY() const { return bearingY; }
	};
} }
