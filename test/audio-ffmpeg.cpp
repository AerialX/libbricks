#include "brickstest.hpp"

#include <bricks/core/autopointer.h>
#include <bricks/core/exception.h>
#include <bricks/audio/ffmpegcodec.h>
#include <bricks/io/filestream.h>

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Audio;

class BricksAudioFFmpegTest : public testing::Test
{
protected:
	AutoPointer<Stream> fileStream;
	virtual void SetUp()
	{
		fileStream = autonew FileStream(TestPath.Combine("data/test.wav"), FileOpenMode::Open, FileMode::ReadOnly);
	}

	virtual void TearDown()
	{
	}
};

TEST_F(BricksAudioFFmpegTest, InitialParse) {
	try {
		autonew FFmpegDecoder(fileStream);
	} catch (const Exception& ex) {
		ADD_FAILURE();
	}
}

TEST_F(BricksAudioFFmpegTest, ReadMetadata) {
	AutoPointer<AudioCodec<s16> > decoder = autonew FFmpegDecoder(fileStream);

	EXPECT_EQ(0x400, decoder->GetSamples());
	EXPECT_EQ(1, decoder->GetChannels());
	EXPECT_EQ(44100, decoder->GetSampleRate());
}

TEST_F(BricksAudioFFmpegTest, ReadSamples) {
	AutoPointer<AudioCodec<s16> > decoder = autonew FFmpegDecoder(fileStream);

	AudioBuffer<s16> buffer(decoder->GetChannels(), 0x100);
	for (u32 channel = 0; channel < decoder->GetChannels(); channel++) {
		for (u32 i = 0; i < decoder->GetSamples() / buffer.GetSize(); i++) {
			EXPECT_EQ(buffer.GetSize(), decoder->Read(buffer, buffer.GetSize()));
			for (u32 k = 0; k < buffer.GetSize(); k++)
				EXPECT_GT(0x10, abs(buffer.GetBuffer()[channel][k]));
		}
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
