#include "bricks.h"
#include "bricks/collections/array.h"
#include "bricks/collections/stack.h"

using namespace Bricks::Collections;

namespace Bricks {
#ifdef BRICKS_CONFIG_RTTI
	Class& Object::GetClass() const
	{
		return AutoAlloc(Class, self);
	}
#endif

	// TODO: Make this thread-safe
	typedef Stack< Pointer< ObjectPool > > PoolStack;
	static Pointer<PoolStack> pools = Alloc(PoolStack);
	static PoolStack& GetThreadPools() { return *pools; }

	ObjectPool& ObjectPool::GetCurrentPool()
	{
		PoolStack& pools = GetThreadPools();
		if (!pools.GetCount())
			throw Alloc(ObjectPoolLeakException);
		return *pools.Peek();
	}

	void ObjectPool::AddObject(Object& object)
	{
		BRICKS_FEATURE_LOG("Autoreleasing %p [%d]", &object, object.GetReferenceCount());
		objects.AddItem(AutoPointer<Object>(&object));
	}

	ObjectPool::ObjectPool() :
		Object(), objects(Alloc(Stack< AutoPointer<Object> >))
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
