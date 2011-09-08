#include "bricksall.hpp"

#ifdef BRICKS_CONFIG_IMAGING_LIBPNG

#include <png.h>

using namespace Bricks::IO;

namespace Bricks { namespace Imaging {
	static void libpng_read(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		Pointer<Stream> stream(static_cast<Stream*>(png_get_io_ptr(png_ptr)));

		if (stream->Read(data, length) != length)
			throw InvalidArgumentException(); // Eh, bad idea to do from inside libpng?
	}

	static void libpng_error(png_structp png_pt, png_const_charp message)
	{
		throw Exception(message);
	}

	// TODO: This should let exceptions propogate past the function, never return NULL
	//       Also catch exceptions and destroy libpng objects before throwing it along to the caller
	ReturnPointer<Bitmap> PNG::LoadImage(const Pointer<Stream>& stream, bool transform)
	{
		png_structp png_ptr;
		png_infop info_ptr;

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, libpng_error, NULL);

		if (!png_ptr)
			return NULL;

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) {
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return NULL;
		}

		png_set_read_fn(png_ptr, static_cast<void*>(stream.GetValue()), libpng_read);
		png_set_keep_unknown_chunks(png_ptr, 1, NULL, 0);

		if (transform)
			png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_SHIFT | PNG_TRANSFORM_STRIP_16, NULL);
		else
			png_read_info(png_ptr, info_ptr);

		u32 width = png_get_image_width(png_ptr, info_ptr);
		u32 height = png_get_image_height(png_ptr, info_ptr);

		u8 bitDepth = transform ? 8 : png_get_bit_depth(png_ptr, info_ptr);
		PixelDescription pixels;
		switch (png_get_color_type(png_ptr, info_ptr)) {
			case PNG_COLOR_TYPE_GRAY:
				pixels = PixelDescription(bitDepth, ColourTypeMask::Intensity);
				break;
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				pixels = PixelDescription(bitDepth, ColourTypeMask::IntensityAlpha);
				break;
			case PNG_COLOR_TYPE_PALETTE:
				pixels = PixelDescription(bitDepth, ColourTypeMask::Palette);
				break;
			case PNG_COLOR_TYPE_RGB:
				pixels = PixelDescription(bitDepth, ColourTypeMask::RGB);
				break;
			case PNG_COLOR_TYPE_RGB_ALPHA:
				pixels = PixelDescription(bitDepth, ColourTypeMask::RGBA);
				break;
			default:
				throw NotSupportedException();
		}

/* I believe libpng handles this for us somewhat
		InterlaceType::Enum interlacing;
		switch (png_get_interlace_type(png_ptr)) {
			case PNG_INTERLACE_NONE:
				interlacing = InterlaceType::None;
				break;
			case PNG_INTERLACE_ADAM7:
				interlacing = InterlaceType::Adam7;
				break;
			default:
				throw NotSupportedException();
		}
*/

		AutoPointer<Bitmap> image = autonew Bitmap(width, height, pixels, InterlaceType::None);

		if (transform) {
			png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
			for (u32 i = 0; i < height; i++)
				memcpy((png_bytep)image->GetImageData() + i * width * image->GetPixelDescription().GetPixelSize(), row_pointers[i], width * image->GetPixelDescription().GetPixelSize());
		} else {
			png_bytep row_pointers[height];

			for (u32 i = 0; i < height; i++)
				row_pointers[i] = (png_bytep)image->GetImageData() + i * width * image->GetPixelDescription().GetPixelSize();

			png_set_rows(png_ptr, info_ptr, row_pointers);
			png_read_update_info(png_ptr, info_ptr);

			png_read_image(png_ptr, row_pointers);
		}

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		return image;
	}
} }

#endif
