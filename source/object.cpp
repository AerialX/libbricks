#include "bricks.h"
#include "bricks/collections/array.h"
#include "bricks/collections/stack.h"
#include "bricks/threading/threadlocalstorage.h"

using namespace Bricks::Collections;
using namespace Bricks::Threading;

namespace Bricks {
	ObjectAlloc Auto;
#ifdef BRICKS_CONFIG_RTTI
	Class& Object::GetClass() const
	{
		return autoalloc Class(self);
	}
#endif

	typedef Stack< Pointer<ObjectPool> > PoolStack;
	static ThreadLocalStorage<PoolStack> pools;
	static PoolStack& GetThreadPools() { if (!pools.HasValue()) pools.SetValue(AutoPointer<PoolStack>(alloc PoolStack(), false)); return pools.GetValue(); }

	ObjectPool& ObjectPool::GetCurrentPool()
	{
		PoolStack& pools = GetThreadPools();
		if (!pools.GetCount())
			throw ObjectPoolLeakException();
		return *pools.Peek();
	}

	void ObjectPool::AddObject(Object& object)
	{
		BRICKS_FEATURE_LOG("Autoreleasing %p [%d]", &object, object.GetReferenceCount());
		objects.AddItem(AutoPointer<Object>(&object));
	}

	ObjectPool::ObjectPool() :
		objects(alloc Stack< AutoPointer<Object> >(NULL))
	{
		BRICKS_FEATURE_LOG("Installing Pool: %p", this);
		GetThreadPools().Push(self);
	}

	ObjectPool::~ObjectPool()
	{
		BRICKS_FEATURE_LOG("Draining Pool: %s", GetDebugString().CString());
		GetThreadPools().RemoveItem(self);
		dynamic_cast<Object&>(objects).Release();
	}
}
