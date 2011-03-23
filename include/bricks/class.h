#pragma once

#include "bricks/object.h"
#include "bricks/string.h"

namespace Bricks {
	class Class : public Object
	{
	private:
		const std::type_info& type;

	public:
		Class(const Object& object) : type(typeid(object)) { }
//		const String& GetName() { return type.name(); }
//		String& GetName() { return Alloc<String>(type.name()).Autorelease(); }

		bool operator==(const Class& rhs) { return type == rhs.type; }
	};
}
