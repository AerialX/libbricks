#include "bricksall.hpp"

namespace Bricks {
	int Value::SizeOfType(ValueType::Enum type)
	{
		switch (type) {
			case ValueType::Pointer:
				return sizeof(void*);
			case ValueType::Boolean:
				return sizeof(bool);
			case ValueType::Byte:
				return 8;
			case ValueType::Int16:
				return 2;
			case ValueType::Int32:
			case ValueType::Float32:
				return 4;
			case ValueType::Int64:
			case ValueType::Float64:
				return 8;
			default:
				return 0;
		}
	}

	void Value::SetData(const void* value)
	{
		memcpy(data, value, GetSize());
	}

	void Value::SetType(ValueType::Enum value)
	{
		type = value;
	}

	void* Value::GetPointerValue() const
	{
		switch (type) {
			case ValueType::Pointer:
				return *(void**)data;
			default:
				throw InvalidArgumentException();
		}
	}

	template<typename U, typename T> static typename SFINAE::EnableIf<SFINAE::IsCompatibleBaseType<U, T>::Value, U>::Type ValueCast(T value)
	{
		return value;
	}

	template<typename U, typename T> static typename SFINAE::EnableIf<!SFINAE::IsCompatibleBaseType<U, T>::Value && (SFINAE::IsSameType<U, void*>::Value || SFINAE::IsSameType<T, void*>::Value), U>::Type ValueCast(T value)
	{
		return (U)(unsigned long)value;
	}

	template<typename T> static T GetValue(ValueType::Enum type, const void* data)
	{
		switch (type) {
			case ValueType::Pointer:
				return ValueCast<T>(*(void**)data);
			case ValueType::Boolean:
				return ValueCast<T>(*(bool*)data);
			case ValueType::Byte:
				return ValueCast<T>(*(u8*)data);
			case ValueType::Int16:
				return ValueCast<T>(*(u16*)data);
			case ValueType::Int32:
				return ValueCast<T>(*(u32*)data);
			case ValueType::Int64:
				return ValueCast<T>(*(u64*)data);
			case ValueType::Float32:
				return ValueCast<T>(*(f32*)data);
			case ValueType::Float64:
				return ValueCast<T>(*(f64*)data);
			default:
				throw InvalidArgumentException();
		}
	}

	template<typename T> static void SetDataValue(ValueType::Enum type, void* data, T value)
	{
		switch (type) {
			case ValueType::Pointer:
				*(void**)data = ValueCast<void*>(value);
				break;
			case ValueType::Boolean:
				*(bool*)data = ValueCast<bool>(value);
				break;
			case ValueType::Byte:
				*(u8*)data = ValueCast<u8>(value);
				break;
			case ValueType::Int16:
				*(u16*)data = ValueCast<u16>(value);
				break;
			case ValueType::Int32:
				*(u32*)data = ValueCast<u32>(value);
				break;
			case ValueType::Int64:
				*(u64*)data = ValueCast<u64>(value);
				break;
			case ValueType::Float32:
				*(f32*)data = ValueCast<f32>(value);
				break;
			case ValueType::Float64:
				*(f64*)data = ValueCast<f64>(value);
				break;
			default:
				throw InvalidArgumentException();
		}
	}

	bool Value::GetBooleanValue() const
	{
		return GetValue<bool>(type, data);
	}

	u8 Value::GetByteValue() const
	{
		return GetValue<u8>(type, data);
	}

	u16 Value::GetInt16Value() const
	{
		return GetValue<u16>(type, data);
	}

	u32 Value::GetInt32Value() const
	{
		return GetValue<u32>(type, data);
	}

	u64 Value::GetInt64Value() const
	{
		return GetValue<u64>(type, data);
	}

	f32 Value::GetFloat32Value() const
	{
		return GetValue<f32>(type, data);
	}

	f64 Value::GetFloat64Value() const
	{
		return GetValue<f64>(type, data);
	}

	void Value::SetValue(const void* value)
	{
		SetDataValue<void*>(type, data, const_cast<void*>(value));
	}

	void Value::SetValue(bool value)
	{
		SetDataValue<bool>(type, data, value);
	}

	void Value::SetValue(u8 value)
	{
		SetDataValue<u8>(type, data, value);
	}

	void Value::SetValue(u16 value)
	{
		SetDataValue<u16>(type, data, value);
	}

	void Value::SetValue(u32 value)
	{
		SetDataValue<u32>(type, data, value);
	}

	void Value::SetValue(u64 value)
	{
		SetDataValue<u64>(type, data, value);
	}

	void Value::SetValue(f32 value)
	{
		SetDataValue<f32>(type, data, value);
	}

	void Value::SetValue(f64 value)
	{
		SetDataValue<f64>(type, data, value);
	}
}
