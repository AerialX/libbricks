#pragma once

#include "bricks/core/object.h"
#include "bricks/core/pointer.h"
#include "bricks/core/string.h"

#if BRICKS_CONFIG_RTTI
#include <typeinfo>

namespace Bricks {
	class TypeInfo : public Object
	{
	private:
		const std::type_info* type;

	public:
		TypeInfo() : type(NULL) { }
		TypeInfo(const TypeInfo& type) : type(type.type) { }
		TypeInfo(const std::type_info& type) : type(&type) { }
		template<typename T> TypeInfo(T* object) : type(&typeid(*object)) { }
		template<typename T> TypeInfo(const Pointer<T>& object) : type(&typeid(*object)) { }

		TypeInfo& operator =(const TypeInfo& rhs) { type = rhs.type; return *this; }

		String GetName() const { return type->name(); }

		template<typename T> static TypeInfo OfType() { return typeid(T); }

		bool operator ==(const TypeInfo& rhs) const { return type && rhs.type && *type == *rhs.type; }
		bool operator !=(const TypeInfo& rhs) const { return !type || !rhs.type || *type != *rhs.type; }
		bool operator >(const TypeInfo& rhs) const { return type && rhs.type && rhs.type->before(*type); }
		bool operator <(const TypeInfo& rhs) const { return type && rhs.type && type->before(*rhs.type); }
		bool operator >=(const TypeInfo& rhs) const { return operator>(rhs) || operator==(rhs); }
		bool operator <=(const TypeInfo& rhs) const { return operator<(rhs) || operator==(rhs); }
	};

	template<typename T> static inline TypeInfo TypeOf(T* t) { return TypeInfo(t); }
	template<typename T> static inline TypeInfo TypeOf(const Pointer<T>& t) { return TypeOf<T>(t.GetValue()); }
}
#endif
