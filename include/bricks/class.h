#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

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
