#include "brickstest.hpp"

TEST(BricksImagingImageTest, ReadPixels32) {
	AutoPointer<BitmapImage> image = autonew Bitmap(0x20, 0x20, PixelDescription::RGBA8);
	u32* data = (u32*)image->GetImageData();
	for (u32 i = 0; i < image->GetWidth() * image->GetHeight(); i++)
		EndianConvertBE32(data + i, 0x8090a0b0);

	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			Colour pixel = image->GetPixel(x, y);
			EXPECT_EQ(Colour(0x80, 0x90, 0xa0, 0xb0), pixel) << "(" << x << ", " << y << ")";
		}
	}
}

TEST(BricksImagingImageTest, ReadPixels24) {
	AutoPointer<BitmapImage> image = autonew Bitmap(0x20, 0x20, PixelDescription::RGB8);
	u8* data = (u8*)image->GetImageData();
	u8 colour[] = { 0x80, 0x90, 0xa0 };
	for (u32 i = 0; i < image->GetWidth() * image->GetHeight() * 3; i++)
		data[i] = colour[i % 3];

	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			Colour pixel = image->GetPixel(x, y);
			EXPECT_EQ(Colour(0x80, 0x90, 0xa0, 0xff), pixel) << "(" << x << ", " << y << ")";
		}
	}
}

TEST(BricksImagingImageTest, WritePixels32) {
	AutoPointer<BitmapImage> image = autonew Bitmap(0x20, 0x20, PixelDescription::RGBA8);
	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			image->SetPixel(x, y, Colour(0x80, 0x90, 0xa0, 0xb0));
		}
	}

	u32* data = (u32*)image->GetImageData();
	for (u32 i = 0; i < image->GetWidth() * image->GetHeight(); i++)
		EXPECT_EQ(0x8090a0b0, EndianConvertBE32(data + i));
}

TEST(BricksImagingImageTest, WritePixels24) {
	AutoPointer<BitmapImage> image = autonew Bitmap(0x20, 0x20, PixelDescription::RGB8);
	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			image->SetPixel(x, y, Colour(0x80, 0x90, 0xa0, 0xb0));
		}
	}

	u8 colour[] = { 0x80, 0x90, 0xa0 };
	u8* data = (u8*)image->GetImageData();
	for (u32 i = 0; i < image->GetWidth() * image->GetHeight() * 3; i++)
		EXPECT_EQ(colour[i % 3], data[i]);
}

TEST(BricksImagingImageTest, ReadWritePixels32) {
	AutoPointer<Image> image = autonew Bitmap(0x20, 0x20, PixelDescription::RGBA8);
	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			image->SetPixel(x, y, Colour(0x80, 0x90, 0xa0, 0xb0));
		}
	}

	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			Colour pixel = image->GetPixel(x, y);
			EXPECT_EQ(Colour(0x80, 0x90, 0xa0, 0xb0), pixel) << "(" << x << ", " << y << ")";
		}
	}
}

TEST(BricksImagingImageTest, CopyPixels32_32) {
	AutoPointer<Image> image = autonew Bitmap(0x20, 0x20, PixelDescription::RGBA8);
	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			image->SetPixel(x, y, Colour(0x80, 0x90, 0xa0, 0xb0));
		}
	}

	AutoPointer<Image> dest = autonew Bitmap(0x20, 0x20, PixelDescription::RGBA8);
	image->CopyTo(dest);

	for (u32 x = 0; x < dest->GetWidth(); x++) {
		for (u32 y = 0; y < dest->GetHeight(); y++) {
			Colour pixel = dest->GetPixel(x, y);
			EXPECT_EQ(Colour(0x80, 0x90, 0xa0, 0xb0), pixel) << "(" << x << ", " << y << ")";
		}
	}
}

TEST(BricksImagingImageTest, CopyPixels32_24) {
	AutoPointer<Image> image = autonew Bitmap(0x20, 0x20, PixelDescription::RGBA8);
	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			image->SetPixel(x, y, Colour(0x80, 0x90, 0xa0, 0xb0));
		}
	}

	AutoPointer<Image> dest = autonew Bitmap(0x20, 0x20, PixelDescription::RGB8);
	image->CopyTo(dest);

	for (u32 x = 0; x < dest->GetWidth(); x++) {
		for (u32 y = 0; y < dest->GetHeight(); y++) {
			Colour pixel = dest->GetPixel(x, y);
			EXPECT_EQ(Colour(0x80, 0x90, 0xa0, 0xff), pixel) << "(" << x << ", " << y << ")";
		}
	}
}

TEST(BricksImagingImageTest, CopyPixels24_32) {
	AutoPointer<Image> image = autonew Bitmap(0x20, 0x20, PixelDescription::RGB8);
	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			image->SetPixel(x, y, Colour(0x80, 0x90, 0xa0, 0xb0));
		}
	}

	AutoPointer<Image> dest = autonew Bitmap(0x20, 0x20, PixelDescription::RGBA8);
	image->CopyTo(dest);

	for (u32 x = 0; x < dest->GetWidth(); x++) {
		for (u32 y = 0; y < dest->GetHeight(); y++) {
			Colour pixel = dest->GetPixel(x, y);
			EXPECT_EQ(Colour(0x80, 0x90, 0xa0, 0xff), pixel) << "(" << x << ", " << y << ")";
		}
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
