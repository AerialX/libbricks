#pragma once

#include "bricks/object.h"

namespace Bricks {
	namespace ValueType {
		enum Enum {
			Pointer,
			Boolean,
			Byte,
			Int16,
			Int32,
			Int64,
			Float32,
			Float64,
			Float = Float32,
			Double = Float64
		};
	}

	class Value : public Object
	{
	protected:
		u8 data[0x08];
		ValueType::Enum type;

	public:
		Value(const void* data, ValueType::Enum type) : type(type) { SetData(data); }
		Value(const void* value) : type(ValueType::Pointer) { SetData(&value); }
		Value(bool value) : type(ValueType::Boolean) { SetData(&value); }
		Value(u8 value) : type(ValueType::Byte) { SetData(&value); }
		Value(u16 value) : type(ValueType::Int16) { SetData(&value); }
		Value(u32 value) : type(ValueType::Int32) { SetData(&value); }
		Value(int value) : type(ValueType::Int32) { SetData(&value); }
		Value(u64 value) : type(ValueType::Int64) { SetData(&value); }
		Value(f32 value) : type(ValueType::Float32) { SetData(&value); }
		Value(f64 value) : type(ValueType::Float64) { SetData(&value); }

		ValueType::Enum GetType() const { return type; }
		int GetSize() const { return SizeOfType(type); }
		const void* GetData() const { return data; }

		void SetData(const void* value);
		void SetType(ValueType::Enum value);

		void* GetPointerValue() const;
		bool GetBooleanValue() const;
		u8 GetByteValue() const;
		u16 GetInt16Value() const;
		u32 GetInt32Value() const;
		u64 GetInt64Value() const;
		f32 GetFloat32Value() const;
		f64 GetFloat64Value() const;
		f32 GetFloatValue() const { return GetFloat32Value(); }
		f32 GetDoubleValue() const { return GetFloat64Value(); }
		int GetIntValue() const { return GetInt32Value(); }

		void SetValue(const void* value);
		void SetValue(bool value);
		void SetValue(u8 value);
		void SetValue(u16 value);
		void SetValue(u32 value);
		void SetValue(u64 value);
		void SetValue(f32 value);
		void SetValue(f64 value);
		void SetValue(int value) { SetValue((u32)value); }

		static int SizeOfType(ValueType::Enum type);
	};
}
