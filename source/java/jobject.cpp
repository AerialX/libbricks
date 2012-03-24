#include "bricks/java/jobject.h"
#include "bricks/java/jclass.h"
#include "bricks/java/jvm.h"
#include "bricks/core/autopointer.h"

namespace Bricks { namespace Java {
	JObject::JObject(JClass* clazz, jobject object, bool global) :
		JReference(clazz->GetVM(), object, global),
		clazz(clazz)
	{

	}

	JObject::JObject(JVM* vm, jobject object, bool global) :
		JReference(vm, object, global),
		clazz(NULL)
	{

	}

	JObject::JObject(JReference* reference) :
		JReference(*reference),
		clazz(NULL)
	{

	}

	JObject::~JObject()
	{

	}

	void JObject::FillClass()
	{
		clazz = autonew JClass(vm, GetObject());
	}

	bool JObject::IsTypeOf(JClass* clazz)
	{
		return GetVM()->GetEnv()->IsInstanceOf(GetObject(), clazz->GetClass());
	}
} }
