#pragma once

#include "bricks/object.h"
#include "bricks/imaging/colour.h"

namespace Bricks { namespace Imaging {
	namespace InterlaceType {
		enum Enum {
			None = 0,
			Adam7
		};
	}

	class Image
	{
	public:
		virtual u32 GetWidth() const = 0;
		virtual u32 GetHeight() const = 0;
		virtual void SetSize(u32 width, u32 height) = 0;

		virtual Colour GetPixel(u32 x, u32 y) const = 0;
		virtual void SetPixel(u32 x, u32 y, const Colour& colour) = 0;

		virtual void CopyTo(const Pointer<Image>& image, u32 x = 0, u32 y = 0) const;
		virtual void CopyTo(const Pointer<Image>& image, u32 x, u32 y, u32 width, u32 height) const;

		void ColouriseTo(const Pointer<Image>& image, const Colour& colour) const;
		void Colourise(const Colour& colour) { ColouriseTo(this, colour); }
	};

	class BitmapImage : public Image
	{
	public:
		virtual void* GetImageData() const = 0;
		virtual u32 GetImageDataSize() const = 0;
		virtual u32 GetImageDataStride() const = 0;
		virtual const PixelDescription& GetPixelDescription() const = 0;
		virtual ReturnPointer<Palette> GetPalette() const = 0;
		virtual InterlaceType::Enum GetInterlaceType() const = 0;
	};
} }
