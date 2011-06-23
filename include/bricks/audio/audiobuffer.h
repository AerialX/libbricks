#pragma once

#include "bricks/object.h"

namespace Bricks { namespace Audio {
	template<typename T>
	class AudioBuffer : public virtual Object
	{
	public:
		typedef T AudioSample;
	
	protected:
		AudioSample** buffer;
		u32 channels;
		u32 size;

	public:
		AudioBuffer(u32 channels, u32 size) : channels(channels), size(size) {
			buffer = new AudioSample*[channels];
			for (u32 i = 0; i < channels; i++)
				buffer[i] = new AudioSample[size];
		}
		~AudioBuffer() {
			for (u32 i = 0; i < channels; i++)
				delete[] buffer[i];
			delete[] buffer;
		}

		AudioBuffer(const AudioBuffer<T>& source) : channels(source.GetChannels()), size(source.GetSize()) {
			buffer = new AudioSample*[channels];
			for (u32 i = 0; i < channels; i++) {
				buffer[i] = new AudioSample[size];
				memcpy(buffer[i], source[i], size * sizeof(AudioSample));
			}
		}

		AudioBuffer<T>& operator=(const AudioBuffer<T>& source) {
			this->~AudioBuffer();
			channels = source.GetChannels();
			size = source.GetSize();
			buffer = new AudioSample*[channels];
			for (u32 i = 0; i < channels; i++) {
				buffer[i] = new AudioSample[size];
				memcpy(buffer[i], source[i], size * sizeof(AudioSample));
			}
		}

		AudioSample* operator[](u32 index) { return buffer[index]; }
		const AudioSample* operator[](u32 index) const { return buffer[index]; }

		u32 GetChannels() const { return channels; }
		u32 GetSize() const { return size; }
		AudioSample** GetBuffer() { return buffer; }
		const AudioSample** GetBuffer() const { return buffer; }

		int CopyTo(AudioBuffer<AudioSample>& dest, u32 sourcechannel = 0, u32 sourceindex = 0, u32 destchannel = 0, u32 destindex = 0, s32 count = -1) const {
			if (count < 0)
				count = size;
			count = BRICKS_FEATURE_MIN(BRICKS_FEATURE_MIN(count, size - sourceindex), dest.GetSize() - destindex);
			for (u32 i = sourcechannel; i < channels; i++)
				memcpy(dest[destchannel + i - sourcechannel] + destindex, buffer[i] + sourceindex, count * sizeof(AudioSample));
			return count;
		}

		void DeinterlaceFrom(const AudioSample* source, u32 count, u32 offset = 0) {
			for (u32 i = 0; i < channels; i++) {
				for (u32 k = 0; k < count; k++)
					buffer[i][k + offset] = source[k * channels + i];
			}
		}

		void InterlaceTo(AudioSample* dest, u32 count, u32 offset = 0) {
			for (u32 k = 0; k < count; k++) {
				for (u32 i = 0; i < channels; i++)
					dest[k * channels + i] = buffer[i][k + offset];
			}
		}

		void Zero() {
			for (u32 i = 0; i < channels; i++)
				memset(buffer[i], 0, size * sizeof(AudioSample));
		}
	};
} }
