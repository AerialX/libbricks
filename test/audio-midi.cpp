#include "brickstest.hpp"

#include <bricks/core/autopointer.h>
#include <bricks/audio/midireader.h>
#include <bricks/io/filestream.h>

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Audio;

class BricksAudioMidiTest : public testing::Test
{
protected:
	AutoPointer<Stream> fileStream;
	virtual void SetUp()
	{
		fileStream = autonew FileStream(TestPath.Combine("data/test.mid"), FileOpenMode::Open, FileMode::ReadOnly);
	}

	virtual void TearDown()
	{
	}
};

TEST_F(BricksAudioMidiTest, Parse) {
	MidiReader reader(fileStream);
	while (!reader.EndOfFile()) {
		while (!reader.EndOfTrack()) {
			AutoPointer<MidiEvent> event = reader.ReadEvent();
		}
		reader.NextTrack();
	}
}

TEST_F(BricksAudioMidiTest, VerifyTrackNames) {
	MidiReader reader(fileStream);
	for (int track = 0; !reader.EndOfFile(); reader.NextTrack(), track++) {
		while (!reader.EndOfTrack()) {
			AutoPointer<MidiEvent> event = reader.ReadEvent();
			if (event->GetType() == MidiEventType::TrackName && track > 0) // Track 0's track name is the MIDI file name
				EXPECT_EQ(String::Format("TRACK %d NAME", track), CastTo<MidiTextEvent>(event)->GetString()) << "Track name did not match";
		}
	}
}

TEST_F(BricksAudioMidiTest, VerifyTrackNotes) {
	MidiReader reader(fileStream);
	for (int track = 0; !reader.EndOfFile(); reader.NextTrack(), track++) {
		int noteCount = 0;
		while (!reader.EndOfTrack()) {
			AutoPointer<MidiEvent> event = reader.ReadEvent();
			if (event->GetType() == MidiEventType::NoteOn) {
				EXPECT_EQ(track, CastTo<MidiNoteEvent>(event)->GetNote()) << "Track note value did not match";
				noteCount++;
			}
		}
		if (track > 0) // Track 0 has no notes, just tempo data
			EXPECT_EQ(4, noteCount) << "Unexpected note count";
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
