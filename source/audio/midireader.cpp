#include "bricks/audio/midireader.h"

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Audio;

static int ReadInteger(Stream& stream)
{
	int ret = 0;
	u8 num;
	do {
		num = stream.ReadByte();
		ret <<= 7;
		ret |= num & 0x7F;
	} while (num & 0x80);
	return ret;
}

static MidiTimeDivision& ReadDivision(StreamReader& reader)
{
	u16 division = reader.ReadInt16();
	if (division & 0x8000)
		return AutoAlloc<MidiFramesPerSecondDivision>((division & 0x7F00) >> 8, division & 0xFF);
	return AutoAlloc<MidiTicksPerBeatDivision>(division & 0x7FFF);
}

template<typename Test> Test* wut(Test& test) { return NULL; }
template<typename Test> Test* wut(const Test& test) { return NULL; }

MidiReader::MidiReader(Stream& stream)
{
	Stream* test = wut(stream);
	reader = TempAlloc<StreamReader>(stream, Endian::BigEndian);

	if (reader->ReadInt32() != MagicHeader1)
		throw FormatException();
	if (reader->ReadInt32() != MagicHeader2)
		throw FormatException();

	midiType = (MidiType::Enum)reader->ReadInt16();
	trackCount = reader->ReadInt16();
	division = ReadDivision(reader);

	trackIndex = 0;
	ReadTrack();

	reader->Retain();
	this->reader = reader;
}

MidiReader::~MidiReader()
{
}

void MidiReader::ReadTrack()
{
	if (reader->ReadInt32() != MagicTrackHeader)
		throw FormatException();

	trackSize = reader->ReadInt32() - sizeof(u32) * 2;

	trackPosition = reader->GetPosition();

	previousIdentifier = 0;
}

void MidiReader::NextTrack()
{
	trackIndex++;

	if (EndOfFile()) {
		trackSize = 0;
		trackPosition = reader->GetPosition();
	} else {
		reader->SetPosition(trackPosition + trackSize);

		ReadTrack();
	}
}

void MidiReader::SeekTrack(u32 index)
{
	if (index >= trackCount)
		throw InvalidArgumentException();

	if (index <= trackIndex) {
		reader->SetPosition(sizeof(MagicHeader1) + sizeof(MagicHeader2) + sizeof(u16) * 3);
		trackIndex = 0;
		ReadTrack();
	}
	while (trackIndex < index)
		NextTrack();
}

Stream& MidiReader::GetTrackStream()
{
	if (EndOfFile())
		throw InvalidOperationException();

	return AutoAlloc<Substream>(reader->GetStream(), trackPosition, trackSize);
}

static MidiMetaEvent& CreateMetaEvent(u32 delta, MidiEventType::Enum type, u32 length, void* data)
{
	MidiMetaEvent event(delta, type, length, data);
	switch (type) {
		case MidiEventType::Comment:
		case MidiEventType::Copyright:
		case MidiEventType::TrackName:
		case MidiEventType::InstrumentName:
		case MidiEventType::Lyric:
		case MidiEventType::Marker:
		case MidiEventType::CuePoint:
			return AutoAlloc<MidiTextEvent>(event);
		case MidiEventType::Tempo:
			return AutoAlloc<MidiTempoEvent>(event);
		case MidiEventType::TimeSignature:
			return AutoAlloc<MidiTimeSignatureEvent>(event);
		default:
			return AutoAlloc<MidiMetaEvent>(event);
	}
}

static MidiChannelEvent& CreateChannelEvent(u32 delta, MidiEventType::Enum type, u8 channel, u8 parameter1, u8 parameter2)
{
	MidiChannelEvent event(delta, type, channel, parameter1, parameter2);
	switch (type) {
		case MidiEventType::NoteOn:
		case MidiEventType::NoteOff:
		case MidiEventType::NoteAftertouch:
			return AutoAlloc<MidiNoteEvent>(event);
		default:
			return AutoAlloc<MidiChannelEvent>(event);
	}
}

MidiEvent& MidiReader::ReadEvent()
{
	if (EndOfTrack())
		throw InvalidOperationException();

	u32 delta = ReadInteger(reader->GetStream());
	u8 identifier = reader->ReadByte();

	switch (identifier) {
		case 0xFF: {
			MidiEventType::Enum type = (MidiEventType::Enum)reader->ReadByte();
			u32 length = ReadInteger(reader->GetStream());
			u8 data[length];
			reader->ReadBytes(data, length);
			return CreateMetaEvent(delta, type, length, data); }
		case 0xF0:
			throw NotImplementedException();
		case 0xF7:
			throw NotImplementedException();
		default: {
			u8 parameter1;
			if (!(identifier & 0x80)) {
				parameter1 = identifier;
				identifier = previousIdentifier;
			} else {
				previousIdentifier = identifier;
				parameter1 = reader->ReadByte();
			}

			MidiEventType::Enum type = (MidiEventType::Enum)(identifier >> 4);
			u8 parameter2 = 0;
			if (type != 0x0C && type != 0x0D)
				parameter2 = reader->ReadByte();

			return CreateChannelEvent(delta, type, identifier & 0x0F, parameter1, parameter2); }
	}
}
