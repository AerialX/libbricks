#include "bricks.h"
#include "bricks/collections/array.h"
#include "bricks/collections/stack.h"
#include "bricks/threading/threadlocalstorage.h"

using namespace Bricks::Collections;
using namespace Bricks::Threading;

namespace Bricks {
	Internal::ObjectAlloc Internal::Auto;

#ifdef BRICKS_CONFIG_RTTI
	Class& Object::GetClass() const
	{
		return AutoAlloc<Class>(*this);
	}
#endif

	typedef Stack< Pointer<ObjectPool> > PoolStack;
	static ThreadLocalStorage<PoolStack> pools;
	static PoolStack& GetThreadPools() { if (!pools.HasValue()) pools.SetValue(TempAlloc<PoolStack>()); return pools.GetValue(); }

#ifdef BRICKS_CONFIG_LOGGING_MEMLEAK
	typedef Array< Pointer<Object> > MemleakArray;
	static Pointer< MemleakArray > memleaks;
	static bool memleaksLock = false; // TODO: True lock from Threading namespace here.
	static bool memleaksAllocLock = false;

#ifdef BRICKS_FEATURE_TLS
	static BRICKS_FEATURE_TLS bool reportobject = false;
	void Object::ReportObject(bool status)
	{
		reportobject = status;
	}
#endif

	void Object::ReportObject(Object* object, bool status)
	{
		if (memleaksAllocLock)
			return;

		if (!memleaks) {
			bool status = reportobject;
			memleaksAllocLock = true;
			memleaks = Alloc<MemleakArray>();
			memleaksAllocLock = false;
			reportobject = status;
		}

		if (!reportobject && status)
			return;

		ReportObject(false);

		if (status) {
			if (!memleaks->ContainsItem(object)) {
				memleaks->AddItem(object);
				object->Retain();
			}
		} else {
			if (memleaks->RemoveItem(object))
				object->Release();
		}
	}

	BRICKS_FEATURE_DESTRUCTOR(MemleakDestructor)
	static void MemleakDestructor()
	{
		if (!memleaks)
			return;

		memleaksAllocLock = true;

		{
			ObjectPool pool;

			if (memleaks->GetCount())
				BRICKS_FEATURE_LOG("MEMORY LEAKS");

			foreach (const Pointer<Object>& object, *memleaks) {
				BRICKS_FEATURE_LOG("= %p [%d] %s", object.GetValue(), object->GetReferenceCount() - 1, object->GetDebugString().CString());
			}

			memleaks->Release();
		}

		memleaksAllocLock = false;
	}
#endif

	ObjectPool& ObjectPool::GetCurrentPool()
	{
		PoolStack& pools = GetThreadPools();
		if (!pools.GetCount())
			throw ObjectPoolLeakException();
		return *pools.Peek();
	}

	void ObjectPool::AddObject(Object& object)
	{
		BRICKS_FEATURE_LOG_HEAVY("Autoreleasing %p [%d]", &object, object.GetReferenceCount());
		objects->AddItem(object);
	}

	ObjectPool::ObjectPool() :
		objects(TempAlloc<Stack< AutoPointer<Object> > >(Pointer<Object>::Null))
	{
		BRICKS_FEATURE_LOG("Installing Pool: %p", this);
		GetThreadPools().Push(*this);
	}

	ObjectPool::~ObjectPool()
	{
		BRICKS_FEATURE_LOG("Draining Pool: %s", GetDebugString().CString());
		GetThreadPools().RemoveItem(*this);
	}
}

extern "C" __attribute__((weak)) const char* _NSPrintForDebugger(Bricks::Object* object)
{
	return object->GetDebugString().CString();
}
