#pragma once

#ifdef BRICKS_CONFIG_RTTI
namespace Bricks {
	class TypeInfo : public Object
	{
	private:
		const std::type_info* type;

	public:
		TypeInfo() : type(NULL) { }
		TypeInfo(const TypeInfo& type) : type(type.type) { }
		TypeInfo(const std::type_info& type) : type(&type) { }
		TypeInfo(const Pointer<const Object>& object) : type(&typeid(*object)) { }
		template<typename T> TypeInfo(const Pointer<T>& object) : type(&typeid(*object)) { }

		TypeInfo& operator=(const TypeInfo& rhs) { type = rhs.type; return *this; }

		String GetName() const { return type->name(); }

		template<typename T> static TypeInfo OfType() { return typeid(T); }

		bool operator==(const TypeInfo& rhs) const { return type && rhs.type && *type == *rhs.type; }
		bool operator!=(const TypeInfo& rhs) const { return !type || !rhs.type || *type != *rhs.type; }
		bool operator>(const TypeInfo& rhs) const { return type && rhs.type && rhs.type->before(*type); }
		bool operator<(const TypeInfo& rhs) const { return type && rhs.type && type->before(*rhs.type); }
	};
}
#endif
