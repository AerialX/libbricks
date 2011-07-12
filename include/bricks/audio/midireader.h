#pragma once

#include "bricks.h"
#include "bricks/io.h"
#include "bricks/audio/miditypes.h"

namespace Bricks { namespace Audio {
	class MidiReader : public virtual Object
	{
	protected:
		AutoPointer<IO::StreamReader> reader;
		MidiType::Enum midiType;
		AutoPointer<MidiTimeDivision> division;
		u16 trackCount;
		u16 trackIndex;
		u32 trackSize;
		u64 trackPosition;
		u8 previousIdentifier;
		
		static const u32 MagicHeader1		= 0x4D546864;
		static const u32 MagicHeader2		= 0x00000006;
		static const u32 MagicTrackHeader	= 0x4D54726B;

		void ReadTrack();

	public:
		MidiReader(IO::Stream& stream);
		~MidiReader();

		bool EndOfFile() const { return trackIndex >= trackCount; }
		bool EndOfTrack() const { return reader->GetPosition() >= trackPosition + trackSize; }

		void NextTrack();
		void SeekTrack(u32 index);
		IO::Stream& GetTrackStream();

		MidiEvent& ReadEvent();

		MidiType::Enum GetMidiType() const { return midiType; }
		u16 GetTrackCount() const { return trackCount; }
		u16 GetCurrentTrack() const { return trackIndex; }
	};
} }
