#include "bricks/object.h"
#include "bricks/objectpool.h"
#include "bricks/class.h"

namespace Bricks {
	Object& Object::Autorelease()
	{
		ObjectPool::GetCurrentPool().AddObject(self);
		Release();
		return self;
	}
	
	Class& Object::GetClass() const
	{
		Class& ret = Alloc<Class>(self);
		ret.Autorelease();
		return ret;
	}
}
