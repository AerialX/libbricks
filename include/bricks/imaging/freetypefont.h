#pragma once

#include "bricks/config.h"

#ifdef BRICKS_CONFIG_IMAGING_FREETYPE

#include "bricks/imaging/font.h"
#include "bricks/io/stream.h"

namespace Bricks { namespace Imaging {
	class FreeTypeFont : public Font
	{
	protected:
		AutoPointer<IO::Stream> stream;
#ifdef FT_FREETYPE_H
		FT_Library library;
		FT_Face face;
		FT_Stream ftstream;
#else
		void* library;
		void* face;
		void* ftstream;
#endif

		ReturnPointer<FontGlyph> LoadGlyph(String::Character character);
		ReturnPointer<Image> RenderGlyph(const Pointer<FontGlyph>& glyph);

	public:
		FreeTypeFont(const Pointer<IO::Stream>& stream, int faceIndex = 0);
		~FreeTypeFont();

		void SetPixelSize(int pixelWidth, int pixelHeight = 0);
		void SetPointSize(int pointWidth, int pointHeight = 0, int dpiWidth = 0, int dpiHeight = 0);

		s32 GetKerning(const Pointer<FontGlyph>& glyph, const Pointer<FontGlyph>& previous);
	};
} }

#endif
