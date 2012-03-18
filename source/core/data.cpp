#include "bricks/core/data.h"
#include "bricks/core/returnpointer.h"
#include "bricks/core/math.h"

namespace Bricks {
	Data::Data(size_t length) :
		data(NULL), length(length), owned(true)
	{
		Construct();
	}

	Data::Data(const void* data, size_t length, bool copy) :
		data(copy ? NULL : (u8*)data), length(length), owned(copy)
	{
		if (copy) {
			Construct();
			CopyFrom(data, length);
		}
	}

	Data::Data(const String& string, bool copy) :
		data(copy ? NULL : (u8*)string.GetBuffer()), length(string.GetSize()), owned(copy)
	{
		if (copy) {
			Construct();
			CopyFrom(string.GetBuffer(), string.GetSize());
		}
	}

	Data::Data(const Data& data, bool copy) :
		data(copy ? NULL : (u8*)data.GetData()), length(data.GetLength()), owned(copy)
	{
		if (copy) { Construct(); CopyFrom(data); }
	}

	Data::~Data()
	{
		if (data && owned)
			delete[] data;
	}

	void Data::Construct()
	{
		if (data && owned)
			delete[] data;
		data = new u8[length];
		owned = true;
	}

	void Data::CopyFrom(const void* value, size_t len, size_t offset)
	{
		memcpy(data + offset, value, Math::Min(len, length - offset));
	}

	void Data::CopyFrom(const Data& value, size_t offset)
	{
		memcpy(data + offset, value.GetData(), Math::Min(value.GetLength(), length - offset));
	}

	Data& Data::operator=(const Data& rhs)
	{
		length = rhs.GetLength();
		Construct();
		CopyFrom(rhs);
		return *this;
	}

	ReturnPointer<Data> Data::Subdata(size_t offset, size_t length) const
	{
		return autonew Data(data + offset, length == String::npos ? this->length : length);
	}
}
