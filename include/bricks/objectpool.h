#pragma once

#include "bricks/types.h"

namespace Bricks {
	class ObjectPool : public Object
	{
	public:
		static ObjectPool& GetCurrentPool();

		void AddObject(Object& object);

		~ObjectPool();
	};
}
