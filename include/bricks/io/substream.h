#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

namespace Bricks { namespace IO {
	class Substream : public Stream
	{
	private:
		AutoPointer<Stream> stream;
		u64 offset;
		u64 position;
		u64 length;

	public:
		Substream(Stream& stream, u64 offset) : stream(stream), offset(offset), position(0) { length = stream.GetLength() - offset; }
		Substream(Stream& stream, u64 offset, u64 length) : stream(stream), offset(offset), position(0), length(0) { }

		u64 GetOffset() { return offset; }
		Stream& GetStream() { return *stream; }

		u64 GetLength() const { return length; }
		void SetLength(u64 length) { this->length = length; }
		u64 GetPosition() const { return position; }
		void SetPosition(u64 position) { this->position = position; }
		void Flush() { stream->Flush(); }

		size_t Read(void* buffer, size_t size) {
			if (position + size > length)
				size = length - position;
			if (stream->GetPosition() != offset + position)
				stream->SetPosition(offset + position);
			size = stream->Read(buffer, size);
			position += size;
			return size;
		}

		size_t Write(const void* buffer, size_t size) {
			if (position + size > length)
				size = length - position;
			if (stream->GetPosition() != offset + position)
				stream->SetPosition(offset + position);
			size = stream->Write(buffer, size);
			position += size;
			return size;
		}

		u64 GetStreamOffset(Pointer<Stream> parent = NULL) {
			Pointer<Substream> substream = dynamic_cast<Substream&>(*stream);
			if (substream && substream != parent)
				return substream->GetStreamOffset(parent) + offset;
			return offset;
		}
	};
} }
