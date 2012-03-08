#include "bricks/core/object.h"
#include "bricks/core/typeinfo.h"
#include "bricks/core/exception.h"

#ifdef BRICKS_CONFIG_LOGGING_MEMLEAK
#include "bricks/collections/array.h"

using namespace Bricks::Collections;
#endif

namespace Bricks {
	namespace Internal {
		ObjectGlobalAlloc Global;
	}

#ifdef BRICKS_CONFIG_RTTI
	String Object::GetDebugString() const
	{
		return String::Format("%s <%p> [%d]", TypeOf(this).GetName().CString(), this, GetReferenceCount());
	}
#else
	String Object::GetDebugString() const
	{
		return String::Format("<%p> [%d]", this, GetReferenceCount());
	}
#endif

#ifndef BRICKS_FEATURE_RELEASE
	void Object::Retain()
	{
		referenceCount++;
		BRICKS_FEATURE_LOG_HEAVY("+ %p [%d]", this, GetReferenceCount());
	}

	void Object::Release()
	{
		BRICKS_FEATURE_LOG_HEAVY("- %p [%d]", this, GetReferenceCount() - 1);
		BRICKS_FEATURE_ASSERT(referenceCount > 0);
		if (!--referenceCount)
			delete this;
		BRICKS_FEATURE_LOGGING_MEMLEAK_DESTROY();
	}
#endif


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
			memleaks = new MemleakArray();
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

		if (memleaks->GetCount())
			BRICKS_FEATURE_LOG("MEMORY LEAKS");

		foreach (const Pointer<Object>& object, memleaks) {
			BRICKS_FEATURE_LOG("= %p [%d] %s", object.GetValue(), object->GetReferenceCount() - 1, object->GetDebugString().CString());
		}

		memleaks->Release();

		memleaksAllocLock = false;
	}
#endif
}

#ifndef BRICKS_FEATURE_APPLE
#ifndef BRICKS_FEATURE_RELEASE
extern "C" __attribute__((weak)) const char* _NSPrintForDebugger(Bricks::Object* object)
{
	return object->GetDebugString().CString();
}
#endif
#endif
