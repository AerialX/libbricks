#pragma once

#include "bricks/types.h"
#include "bricks/object.h"

namespace Bricks {
	namespace Collections { template<typename T> class Collection; }

	class ObjectPool : public virtual Object
	{
	private:
		AutoPointer<Collections::Collection< AutoPointer<Object> > > objects;

	public:
		static ObjectPool& GetCurrentPool();

		void AddObject(Object& object);

		ObjectPool();
		~ObjectPool();
	};

	inline Object& Object::Autorelease()
	{
		ObjectPool::GetCurrentPool().AddObject(*this);
		Release();
		return *this;
	}
}
