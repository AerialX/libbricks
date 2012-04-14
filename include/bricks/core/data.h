#pragma once

#include "bricks/core/object.h"

namespace Bricks {
	template<typename T> class ReturnPointer;
	class String;

	class Data : public Object
	{
	protected:
		u8* data;
		size_t length;
		bool owned;

		void Construct();

	public:
		Data(size_t length);
		Data(const void* data, size_t length, bool copy = true);
		Data(const String& string, bool copy = true);
		Data(const Data& data, bool copy = true);

		~Data();

		void CopyFrom(const void* value, size_t len, size_t offset = 0);
		void CopyFrom(const Data& value, size_t offset = 0);

		Data& operator=(const Data& rhs);

		ReturnPointer<Data> Subdata(size_t offset = 0, size_t length = -1) const;

		size_t GetLength() const { return length; }
		size_t GetSize() const { return length; }
		void SetSize(size_t value) { length = value; }
		const void* GetData() const { return data; }
		void* GetData() { return data; }
		operator const void*() const { return data; }
		operator void*() { return data; }
		u8& operator[](size_t index) { return data[index]; }
		const u8& operator[](size_t index) const { return data[index]; }
		u8 GetValue(size_t index) const { return data[index]; }
		void SetValue(size_t index, u8 value) { data[index] = value; }
	};
}
