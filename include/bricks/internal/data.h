#pragma once

#include "bricks/object.h"

namespace Bricks {
	class Data : public Object
	{
	protected:
		u8* data;
		size_t length;
		bool owned;

	public:
		Data(size_t length) : length(length) { Construct(); }
		Data(const void* data, size_t length, bool copy = true) : data(copy ? NULL : (u8*)data), length(length), owned(!copy) { if (copy) { Construct(); CopyFrom(data, length); } }
		Data(const String& string, bool copy = true) : data(copy ? NULL : (u8*)string.GetBuffer()), length(string.GetLength()), owned(!copy) { if (copy) { Construct(); CopyFrom(string.GetBuffer(), string.GetLength()); } }
		Data(const Data& data, bool copy = true) : data(copy ? NULL : (u8*)data.GetData()), length(data.GetLength()), owned(!copy) { if (copy) { Construct(); CopyFrom(data); } }

		~Data() { if (data && owned) delete[] data; }

		void Construct() { if (data && owned) delete[] data; data = new u8[length]; owned = true; }

		void CopyFrom(const void* value, size_t len, size_t offset = 0) { memcpy(data + offset, value, BRICKS_FEATURE_MIN(len, length - offset)); }
		void CopyFrom(const Data& value, size_t offset = 0) { memcpy(data + offset, value.GetData(), BRICKS_FEATURE_MIN(value.GetLength(), length - offset)); }

		Data& operator=(const Data& rhs) { length = rhs.GetLength(); Construct(); CopyFrom(rhs); return *this; }

		size_t GetLength() const { return length; }
		const void* GetData() const { return data; }
		void* GetData() { return data; }
		operator const void*() const { return data; }
		operator void*() { return data; }
		u8& operator[](size_t index) { return data[index]; }
		const u8& operator[](size_t index) const { return data[index]; }

		ReturnPointer<Data> Subdata(size_t offset = 0, size_t length = String::npos) const { return autonew Data(*this); }
	};
}
