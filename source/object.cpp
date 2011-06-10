#include "bricks.h"
#include "bricks/collections/array.h"
#include "bricks/collections/stack.h"

using namespace Bricks::Collections;

namespace Bricks {
	ObjectAlloc Auto;
#ifdef BRICKS_CONFIG_RTTI
	Class& Object::GetClass() const
	{
		return autoalloc Class(self);
	}
#endif

	// TODO: Make this thread-safe
	typedef Stack< Pointer<ObjectPool> > PoolStack;
	static Pointer< OperatorEqualityComparison< Pointer<ObjectPool> > > PoolComparison = alloc OperatorEqualityComparison< Pointer<ObjectPool> >();
	static Pointer<PoolStack> pools = alloc PoolStack(PoolComparison);
	static PoolStack& GetThreadPools() { return *pools; }

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
