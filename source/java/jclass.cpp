#include "bricks/java/jclass.h"
#include "bricks/java/jobject.h"
#include "bricks/java/jfield.h"
#include "bricks/java/jmethod.h"
#include "bricks/java/jvm.h"
#include "bricks/core/autopointer.h"

namespace Bricks { namespace Java {
	JClass::JClass(JVM* vm, jclass clazz, bool global) :
		JReference(vm, clazz, global)
	{

	}

	JClass::JClass(JVM* vm, jobject object) :
		JReference(vm, vm->GetEnv()->GetObjectClass(object), false)
	{

	}

	JClass::JClass(JReference* reference) :
		JReference(*reference)
	{

	}

	JClass::~JClass()
	{

	}

	ReturnPointer<JClass> JClass::GetSuperclass()
	{
		return autonew JClass(vm, vm->GetEnv()->GetSuperclass(GetClass()));
	}

	ReturnPointer<JObject> JClass::Allocate()
	{
		return autonew JObject(this, vm->GetEnv()->AllocObject(GetClass()));
	}

	ReturnPointer<JField> JClass::GetField(const String& name, const String& signature)
	{
		jfieldID field = vm->GetEnv()->GetFieldID(GetClass(), name.CString(), signature.CString());
		if (!field)
			return NULL;
		return autonew JField(this, field);
	}

	ReturnPointer<JStaticField> JClass::GetStaticField(const String& name, const String& signature)
	{
		jfieldID field = vm->GetEnv()->GetStaticFieldID(GetClass(), name.CString(), signature.CString());
		if (!field)
			return NULL;
		return autonew JStaticField(this, field);
	}

	ReturnPointer<JMethod> JClass::GetMethod(const String& name, const String& signature)
	{
		jmethodID method = vm->GetEnv()->GetMethodID(GetClass(), name.CString(), signature.CString());
		if (!method)
			return NULL;
		return autonew JMethod(this, method);
	}

	ReturnPointer<JStaticMethod> JClass::GetStaticMethod(const String& name, const String& signature)
	{
		jmethodID method = vm->GetEnv()->GetStaticMethodID(GetClass(), name.CString(), signature.CString());
		if (!method)
			return NULL;
		return autonew JStaticMethod(this, method);
	}

	ReturnPointer<JNonvirtualMethod> JClass::GetConstructor(const String& signature)
	{
		jmethodID method = vm->GetEnv()->GetMethodID(GetClass(), "<init>", signature.CString());
		if (!method)
			return NULL;
		return autonew JNonvirtualMethod(this, method);
	}
} }
