#include "brickstest.hpp"

class BricksImagingPngTest : public testing::Test
{
protected:
	AutoPointer<Stream> fileStream;
	virtual void SetUp()
	{
		fileStream = autonew FileStream(TestPath.Combine("data/testFACEB0.png"), FileOpenMode::Open, FileMode::ReadOnly);
	}

	virtual void TearDown()
	{
	}
};

#ifdef BRICKS_CONFIG_IMAGING_LIBPNG

TEST_F(BricksImagingPngTest, InitialParse) {
	try {
		EXPECT_TRUE(PNG::LoadImage(fileStream));
	} catch (const Exception& ex) {
		ADD_FAILURE();
	}
}

TEST_F(BricksImagingPngTest, ReadPixels) {
	AutoPointer<Image> image = PNG::LoadImage(fileStream);
	if (!image)
		FAIL();

	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			Colour pixel = image->GetPixel(x, y);
			EXPECT_EQ(Colour(0xfa, 0xce, 0xb0, 0xff), pixel);
		}
	}

	EXPECT_EQ(0x40 * 0x40, image->GetWidth() * image->GetHeight());
}

TEST_F(BricksImagingPngTest, ReadTransformedPixels) {
	AutoPointer<Image> image = PNG::LoadImage(fileStream, true);
	if (!image)
		FAIL();

	for (u32 x = 0; x < image->GetWidth(); x++) {
		for (u32 y = 0; y < image->GetHeight(); y++) {
			Colour pixel = image->GetPixel(x, y);
			EXPECT_EQ(Colour(0xfa, 0xce, 0xb0, 0xff), pixel);
		}
	}

	EXPECT_EQ(0x40 * 0x40, image->GetWidth() * image->GetHeight());
}

#endif

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
