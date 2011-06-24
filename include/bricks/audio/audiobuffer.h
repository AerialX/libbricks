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

		void Construct() {
			buffer = new AudioSample*[channels];
			for (u32 i = 0; i < channels; i++)
				buffer[i] = new AudioSample[size];
		}

		void Destruct(u32 channels, u32 size) {
			for (u32 i = 0; i < channels; i++)
				delete[] buffer[i];
			delete[] buffer;
		}

	public:
		AudioBuffer(u32 channels, u32 size) : channels(channels), size(size) { Construct(); }
		~AudioBuffer() { Destruct(); }

		AudioBuffer(const AudioBuffer<T>& source) : channels(source.GetChannels()), size(source.GetSize()) {
			Construct();
			source.CopyTo(self, 0, 0, 0, 0, size);
		}

		template<typename Tsrc>
		AudioBuffer(const AudioBuffer<Tsrc>& source) : channels(source.GetChannels()), size(source.GetSize()) {
			Construct();
			source.CopyTo(self, 0, 0, 0, 0, size);
		}

		AudioBuffer<T>& operator=(const AudioBuffer<T>& source) {
			Destruct();
			channels = source.GetChannels();
			size = source.GetSize();
			Construct();
			source.CopyTo(self, 0, 0, 0, 0, size);
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

		template<typename Tsrc>
		int CopyTo(AudioBuffer<Tsrc>& dest, u32 sourcechannel = 0, u32 sourceindex = 0, u32 destchannel = 0, u32 destindex = 0, s32 count = -1) const {
			if (count < 0)
				count = size;
			count = BRICKS_FEATURE_MIN(BRICKS_FEATURE_MIN(count, size - sourceindex), dest.GetSize() - destindex);
			for (u32 i = sourcechannel; i < channels; i++) {
				for (u32 k = 0; k < count; k++)
					dest[destchannel + i - sourcechannel][destindex + k] = ConvertSample(buffer[i][sourceindex + k]);
			}
			return count;
		}

		template<typename Tsrc>
		static T ConvertSample(Tsrc sample) {
			return static_cast<T>(sample);
		}

		template<typename Tsrc>
		static T ConvertSample(Tsrc sample, typename SFINAE::EnableIf<SFINAE::IsIntegerNumber<T>::Value && SFINAE::IsIntegerNumber<Tsrc>::Value>::Type* dummy = NULL) {
			return static_cast<T>(sample * SFINAE::NumericTraits<T>::MaximumValue / SFINAE::NumericTraits<Tsrc>::MaximumValue);
		}

		template<typename Tsrc>
		static T ConvertSample(Tsrc sample, typename SFINAE::EnableIf<SFINAE::IsFloatingPointNumber<T>::Value && SFINAE::IsIntegerNumber<Tsrc>::Value>::Type* dummy = NULL) {
			return static_cast<T>(sample) / SFINAE::NumericTraits<Tsrc>::MaximumValue;
		}

		template<typename Tsrc>
		static T ConvertSample(Tsrc sample, typename SFINAE::EnableIf<SFINAE::IsFloatingPointNumber<Tsrc>::Value && SFINAE::IsIntegerNumber<T>::Value>::Type* dummy = NULL) {
			return static_cast<T>(sample * SFINAE::NumericTraits<T>::MaximumValue);
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
