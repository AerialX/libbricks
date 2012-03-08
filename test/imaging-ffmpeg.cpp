#include "brickstest.hpp"

#include <bricks/core/autopointer.h>
#include <bricks/core/exception.h>
#include <bricks/imaging/ffmpegvideodecoder.h>
#include <bricks/imaging/bitmap.h>
#include <bricks/io/filestream.h>

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Imaging;

class BricksVideoFFmpegTest : public testing::Test
{
protected:
	AutoPointer<Stream> fileStream;
	virtual void SetUp()
	{
		fileStream = autonew FileStream(TestPath.Combine("data/test.mkv"), FileOpenMode::Open, FileMode::ReadOnly);
	}

	virtual void TearDown()
	{
	}
};

TEST_F(BricksVideoFFmpegTest, InitialParse) {
	try {
		autonew FFmpegVideoDecoder(fileStream);
	} catch (const Exception& ex) {
		ADD_FAILURE();
	}
}

TEST_F(BricksVideoFFmpegTest, ReadMetadata) {
	AutoPointer<VideoCodec> decoder = autonew FFmpegVideoDecoder(fileStream);

	EXPECT_EQ(640, decoder->GetFrameWidth());
	EXPECT_EQ(480, decoder->GetFrameHeight());
	EXPECT_EQ(25, (int)decoder->GetFrameRate());
}

TEST_F(BricksVideoFFmpegTest, ReadSamples) {
	AutoPointer<VideoCodec> decoder = autonew FFmpegVideoDecoder(fileStream);

	EXPECT_TRUE(decoder->Read());
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
