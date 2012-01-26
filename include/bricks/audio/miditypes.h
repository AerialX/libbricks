#pragma once

#include "bricks/core/object.h"
#include "bricks/io/endian.h"

namespace Bricks { namespace Audio {
	namespace MidiType {
		enum Enum {
			SingleTrack	= 0,
			Uniform		= 1,
			Hybrid		= 2
		};
	}

	namespace MidiEventType {
		enum Enum {
			Comment				= 0x01,
			Copyright			= 0x02,
			TrackName			= 0x03,
			InstrumentName		= 0x04,
			Lyric				= 0x05,
			Marker				= 0x06,
			CuePoint			= 0x07,

			NoteOff				= 0x08,
			NoteOn				= 0x09,
			NoteAftertouch		= 0x0a,
			Controller			= 0x0b,
			ProgramChange		= 0x0c,
			ChannelAftertouch	= 0x0d,
			PitchBend			= 0x0f,

			EndTrack			= 0x2f,

			Tempo				= 0x51,
			SMPTEOffset			= 0x54,
			TimeSignature		= 0x58,
			KeySignature		= 0x59,
			VendorEvent			= 0x7F
		};
	}

	struct MidiEvent : public Object
	{
	protected:
		u32 deltaTime;
		MidiEventType::Enum type;

	public:
		MidiEvent(u32 deltaTime, MidiEventType::Enum type) : deltaTime(deltaTime), type(type) { }

		u32 GetDeltaTime() const { return deltaTime; }
		MidiEventType::Enum GetType() const { return type; }
		void SetType(MidiEventType::Enum value) { type = value; }
	};

	struct MidiMetaEvent : public MidiEvent
	{
	protected:
		u32 length;
		u8* data;

	public:
		MidiMetaEvent(u32 deltaTime, MidiEventType::Enum type, u32 length, const void* data) : MidiEvent(deltaTime, type), length(length) {
			this->data = new u8[length];
			if (data)
				memcpy(this->data, data, length);
		}

		MidiMetaEvent(const MidiMetaEvent& event) : MidiEvent(event) {
			length = event.length;
			data = new u8[length];
			memcpy(data, event.data, length);
		}

		~MidiMetaEvent() { delete[] data; }

		MidiMetaEvent& operator =(const MidiMetaEvent& event) {
			MidiEvent::operator=(event);
			length = event.length;
			delete[] data;
			data = new u8[length];
			memcpy(data, event.data, length);
			return *this;
		}

		const u8* GetData() const { return data; }
		u32 GetLength() const { return length; }
	};

	struct MidiChannelEvent : public MidiEvent
	{
	protected:
		u8 channel;
		u8 parameter1;
		u8 parameter2;

	public:
		MidiChannelEvent(u32 deltaTime, MidiEventType::Enum type, u8 channel, u8 parameter1, u8 parameter2) : MidiEvent(deltaTime, type), channel(channel), parameter1(parameter1), parameter2(parameter2) { }

		u8 GetChannel() const { return channel; }
		u8 GetParameter1() const { return parameter1; }
		u8 GetParameter2() const { return parameter2; }
	};

	struct MidiTimeDivision : public Object
	{

	};

	struct MidiTicksPerBeatDivision : public MidiTimeDivision
	{
	protected:
		u16 ticksPerBeat;

	public:
		MidiTicksPerBeatDivision(u16 ticksPerBeat) : ticksPerBeat(ticksPerBeat) { }

		u16 GetTicksPerBeat() const { return ticksPerBeat; }
	};

	struct MidiFramesPerSecondDivision : public MidiTimeDivision
	{
	protected:
		u8 framesPerSecond;
		u8 ticksPerFrame;

	public:
		MidiFramesPerSecondDivision(u8 framesPerSecond, u8 ticksPerFrame) : framesPerSecond(framesPerSecond), ticksPerFrame(ticksPerFrame) { }

		u8 GetFramesPerSecond() const { return framesPerSecond; }
		u8 GetTicksPerFrame() const { return ticksPerFrame; }
	};

	struct MidiNoteEvent : public MidiChannelEvent
	{
	public:
		MidiNoteEvent(const MidiChannelEvent& event) : MidiChannelEvent(event) { }

		u8 GetNote() const { return parameter1; }
		u8 GetVelocity() const { return parameter2; }
	};

	struct MidiTextEvent : public MidiMetaEvent
	{
	public:
		MidiTextEvent(const MidiMetaEvent& event) : MidiMetaEvent(event) { }

		String GetString() const { return String((const char*)data, length); }
	};

	struct MidiTempoEvent : public MidiMetaEvent
	{
		MidiTempoEvent(u32 deltaTime, u32 mpqn) : MidiMetaEvent(deltaTime, MidiEventType::Tempo, 3, NULL) { u8 subdata[4]; Bricks::IO::EndianConvertBE32(subdata, mpqn); memcpy(data, subdata + 1, 3); }
		MidiTempoEvent(const MidiMetaEvent& event) : MidiMetaEvent(event) { }
		
		static const u32 MicrosecondsPerMinute = 60000000;
		
		u32 GetMicrosecondsPerQuarterNote() const { u8 subdata[4]; subdata[0] = 0; memcpy(subdata + 1, data, 3); return Bricks::IO::EndianConvertBE32(subdata); }
		float GetBeatsPerMinute() const { return (float)MicrosecondsPerMinute / GetMicrosecondsPerQuarterNote(); }
	};

	struct MidiTimeSignatureEvent : public MidiMetaEvent
	{
		MidiTimeSignatureEvent(const MidiMetaEvent& event) : MidiMetaEvent(event) { }
	};
} }
