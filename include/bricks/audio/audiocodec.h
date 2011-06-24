#pragma once

#include "bricks/audio/audiobuffer.h"

namespace Bricks { namespace Audio {
	template<typename T>
	class AudioCodec : public virtual Object
	{
	protected:
		u32 channels;
		u32 samplerate;
		s64 samples;
		u32 bitrate;
		s64 position;

		AudioCodec(u32 channels = 0, u32 samplerate = 0, s64 samples = 0, u32 bitrate = 0, s64 position = 0) :
			channels(channels), samplerate(samplerate), samples(samples), bitrate(bitrate), position(position) { }

	public:
		virtual u32 GetChannels() const { return channels; }
		virtual u32 GetSampleRate() const { return samplerate; }
		virtual u32 GetBitRate() const { return bitrate; }
		virtual s64 GetSamples() const { return samples; }
		virtual s64 GetPosition() const { return position; }

		virtual u32 Read(AudioBuffer<T>& buffer, u32 count, u32 offset = 0) { position += count; return count; }
		virtual void Seek(s64 sample) { position = sample; }
		virtual void Write(const AudioBuffer<T>& buffer, u32 count, u32 offset = 0) { position += count; }
	};

	template<typename T>
	class NullCodec : public AudioCodec<T>
	{
	public:
		NullCodec(u32 channels, u32 samplerate, s64 samples, u32 bitrate = 0, s64 position = 0) : AudioCodec<T>(channels, samplerate, samples, bitrate, position) { }

		u32 Read(AudioBuffer<T>& buffer, u32 count, u32 offset = 0) {
			count = BRICKS_FEATURE_MIN(count, this->samples - this->position);
			for (u32 i = 0; i < this->channels; i++)
				memset(buffer[i], 0, count * sizeof(AudioBuffer<T>::AudioSample));
			AudioCodec<T>::Read(buffer, count, offset);
			return count;
		}
	};

	template<typename T>
	class Subcodec : public AudioCodec<T>
	{
	protected:
		AutoPointer< AudioCodec<T> > codec;

	public:
		Subcodec(AudioCodec<T>& codec) : codec(codec) { }

		u32 GetChannels() const { return codec->GetChannels(); }
		u32 GetSampleRate() const { return codec->GetSampleRate(); }
		u32 GetBitRate() const { return codec->GetBitRate(); }
		s64 GetSamples() const { return codec->GetSamples(); }
		s64 GetPosition() const { return codec->GetPosition(); }
		void Seek(s64 sample) { codec->Seek(sample); }
		void Write(const AudioBuffer<T>& buffer, u32 count, u32 offset = 0) { codec->Write(buffer, count, offset); }
		u32 Read(AudioBuffer<T>& buffer, u32 count, u32 offset = 0) { return codec->Read(buffer, count, offset); }
	};

	template<typename T>
	class AmplifyCodec : public Subcodec<T>
	{
	protected:
		float amplification;

		void AmplifyBuffer(AudioBuffer<T>& buffer, u32 count, u32 offset) {
			for (u32 i = 0; i < this->GetChannels(); i++) {
				for (u32 k = 0; k < count; k++)
					buffer[i][k + offset] *= amplification;
			}
		}

	public:
		AmplifyCodec(AudioCodec<T>& codec, float amplification) : Subcodec<T>(codec), amplification(amplification) { }

		float GetAmplification() const { return amplification; }
		void SetAmplification(float value) { amplification = value; }

		u32 Read(AudioBuffer<T>& buffer, u32 count, u32 offset = 0) {
			count = Subcodec<T>::Read(buffer, count, offset);
			AmplifyBuffer(buffer, count, offset);
			return count;
		}

		void Write(const AudioBuffer<T>& buffer, u32 count, u32 offset = 0) {
			AudioBuffer<T> newbuffer(this->GetChannels(), count);
			buffer.CopyTo(newbuffer, 0, offset, 0, 0, count);
			AmplifyBuffer(newbuffer, count, offset); // TODO: Reverse?
			Subcodec<T>::Write(newbuffer, count, offset);
		}
	};

	template<typename Tdest, typename Tsrc>
	class ConversionCodec : public AudioCodec<Tdest>
	{
	protected:
		AutoPointer< AudioCodec<Tsrc> > codec;

	public:
		ConversionCodec(AudioCodec<Tsrc>& codec) : codec(codec) { }

		u32 GetChannels() const { return codec->GetChannels(); }
		u32 GetSampleRate() const { return codec->GetSampleRate(); }
		u32 GetBitRate() const { return codec->GetBitRate(); }
		s64 GetSamples() const { return codec->GetSamples(); }
		s64 GetPosition() const { return codec->GetPosition(); }
		void Seek(s64 sample) { codec->Seek(sample); }

		void Write(const AudioBuffer<Tdest>& buffer, u32 count, u32 offset = 0) {
			AudioBuffer<Tsrc> source(buffer);
			codec->Write(source, count, offset);
		}

		u32 Read(AudioBuffer<Tdest>& buffer, u32 count, u32 offset = 0) {
			AudioBuffer<Tsrc> source(buffer.GetChannels(), buffer.GetSize());
			u32 read = codec->Read(source, count, offset);
			buffer = source;
			return read;
		}
	};
} }
