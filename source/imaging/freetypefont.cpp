#include <ft2build.h>
#include FT_FREETYPE_H

#include "bricksall.hpp"

using namespace Bricks::IO;

namespace Bricks { namespace Imaging {
	static unsigned long FreeTypeFontRead(FT_Stream data, unsigned long offset, unsigned char* buffer, unsigned long count)
	{
		Pointer<Stream> stream = static_cast<Stream*>(data->descriptor.pointer);

		if (stream->GetPosition() != offset)
			stream->SetPosition(offset);

		if (!count)
			return 0;

		return stream->Read(buffer, count);
	}

	FreeTypeFont::FreeTypeFont(const Pointer<Stream>& stream, int faceIndex) : stream(stream)
	{
		if (FT_Init_FreeType(&library))
			BRICKS_FEATURE_THROW(Exception());
		FT_Open_Args args;
		ftstream = new FT_StreamRec();
		memset(&args, 0, sizeof(args));
		memset(ftstream, 0, sizeof(FT_StreamRec));
		args.flags = FT_OPEN_STREAM;
		args.stream = ftstream;
		ftstream->descriptor.pointer = static_cast<void*>(stream.GetValue());
		ftstream->read = &FreeTypeFontRead;
		ftstream->size = stream->GetLength();
		if (FT_Open_Face(library, &args, faceIndex, &face)) {
			FT_Done_FreeType(library);
			BRICKS_FEATURE_THROW(Exception());
		}
	}

	FreeTypeFont::~FreeTypeFont()
	{
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		delete ftstream;
	}

	void FreeTypeFont::SetPixelSize(int pixelWidth, int pixelHeight)
	{
		if (FT_Set_Pixel_Sizes(face, pixelWidth, pixelHeight))
			BRICKS_FEATURE_THROW(NotSupportedException());
		Font::SetPixelSize(pixelWidth, pixelHeight);
	}

	void FreeTypeFont::SetPointSize(int pointWidth, int pointHeight, int dpiWidth, int dpiHeight)
	{
		if (FT_Set_Char_Size(face, pointWidth << 6, pointHeight << 6, dpiWidth, dpiHeight))
			BRICKS_FEATURE_THROW(NotSupportedException());
		Font::SetPixelSize(face->size->metrics.x_ppem, face->size->metrics.y_ppem);
	}

	ReturnPointer<FontGlyph> FreeTypeFont::LoadGlyph(String::Character character)
	{
		int index = FT_Get_Char_Index(face, character);
		if (FT_Load_Glyph(face, index, FT_LOAD_DEFAULT))
			BRICKS_FEATURE_THROW(Exception());
		return autonew FontGlyph(this, character, index, face->glyph->advance.x >> 6, face->glyph->metrics.width >> 6, face->glyph->metrics.height >> 6, face->glyph->metrics.horiBearingX >> 6, face->glyph->metrics.horiBearingY >> 6);
	}

	ReturnPointer<Image> FreeTypeFont::RenderGlyph(const Pointer<FontGlyph>& glyph)
	{
		if (FT_Load_Glyph(face, glyph->GetIndex(), FT_LOAD_DEFAULT))
			BRICKS_FEATURE_THROW(Exception());
		FT_GlyphSlot slot = face->glyph;
		if (FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL)) // TODO: FT_RENDER_MODE_LCD / FT_RENDER_MODE_LCD_V ?
			BRICKS_FEATURE_THROW(Exception());
		PixelDescription description;
		switch (slot->bitmap.pixel_mode) {
			case FT_PIXEL_MODE_MONO:
				description = PixelDescription(1, ColourTypeMask::Intensity);
				break;
			case FT_PIXEL_MODE_GRAY:
			case FT_PIXEL_MODE_LCD:
			case FT_PIXEL_MODE_LCD_V:
				description = PixelDescription::I8;
				break;
			default:
				BRICKS_FEATURE_THROW(NotSupportedException());
		}
		AutoPointer<Bitmap> image = autonew Bitmap(slot->bitmap.width, slot->bitmap.rows, description);
		memcpy(image->GetImageData(), slot->bitmap.buffer, image->GetImageDataSize());
		return image;
	}

	s32 FreeTypeFont::GetKerning(const Pointer<FontGlyph>& glyph, const Pointer<FontGlyph>& previous)
	{
		if (!previous || !glyph)
			return 0;
		FT_Vector kerning;
		FT_Get_Kerning(face, previous->GetIndex(), glyph->GetIndex(), FT_KERNING_DEFAULT, &kerning);
		return kerning.x >> 6;
	}

	s32 FreeTypeFont::GetBaseline() const
	{
		return face->size->metrics.ascender >> 6;
	}

	s32 FreeTypeFont::GetDescender() const
	{
		return face->size->metrics.descender >> 6;
	}

	u32 FreeTypeFont::GetHeight() const
	{
		return face->size->metrics.height >> 6;
	}
} }
