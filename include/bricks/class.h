#pragma once

#include "bricks/object.h"

#ifdef BRICKS_CONFIG_RTTI
namespace Bricks {
	class Class : public virtual Object
	{
	private:
		const std::type_info& type;

	public:
		Class(const Object& object) : type(typeid(object)) { }
		const String GetName() { return type.name(); }

		bool operator==(const Class& rhs) { return type == rhs.type; }
	};
}
#endif
