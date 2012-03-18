#pragma once

#include "bricks/core/autopointer.h"
#include "bricks/audio/miditypes.h"

namespace Bricks { namespace IO { class Stream; class StreamReader; } }
namespace Bricks { template<typename T> class ReturnPointer; }

namespace Bricks { namespace Audio {
	class MidiReader : public Object
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
		MidiReader(IO::Stream* stream);
		~MidiReader();

		bool EndOfFile() const;
		bool EndOfTrack() const;

		void NextTrack();
		void SeekTrack(u32 index);
		ReturnPointer<IO::Stream> GetTrackStream();

		ReturnPointer<MidiEvent> ReadEvent();

		MidiTimeDivision* GetDivision() const { return division; }
		MidiType::Enum GetMidiType() const { return midiType; }
		u16 GetTrackCount() const { return trackCount; }
		u16 GetCurrentTrack() const { return trackIndex; }
	};
} }
