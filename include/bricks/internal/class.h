#pragma once

#ifdef BRICKS_CONFIG_RTTI
namespace Bricks {
	class Class : public Object
	{
	private:
		const std::type_info& type;

	public:
		Class(const Pointer<const Object>& object) : type(typeid(*object)) { }
		String GetName() { return type.name(); }

		bool operator==(const Class& rhs) { return type == rhs.type; }
	};
}
#endif
