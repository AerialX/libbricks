#include "brickstest.hpp"

class BricksImagingFreeTypeFontTest : public testing::Test
{
protected:
	AutoPointer<Stream> fileStream;
	virtual void SetUp()
	{
		fileStream = autonew FileStream(TestPath.Combine("data/test.ttf"), FileOpenMode::Open, FileMode::ReadOnly);
	}

	virtual void TearDown()
	{
	}
};

#ifdef BRICKS_CONFIG_IMAGING_FREETYPE

TEST_F(BricksImagingFreeTypeFontTest, InitialParse) {
	try {
		AutoPointer<FreeTypeFont> font = autonew FreeTypeFont(fileStream);
		font->SetPixelSize(0x10);
	} catch (const Exception& ex) {
		ADD_FAILURE();
	}
}

#endif

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
