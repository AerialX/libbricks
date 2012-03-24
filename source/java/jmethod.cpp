#include "bricks/java/jmethod.h"
#include "bricks/java/jobject.h"
#include "bricks/java/jvm.h"
#include "bricks/core/autopointer.h"

namespace Bricks { namespace Java {
	JMethod::JMethod(JClass* clazz, jmethodID method) :
		clazz(clazz), method(method)
	{

	}

	JMethod::~JMethod()
	{

	}

	ReturnPointer<JObject> JMethod::GetReflection()
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->ToReflectedMethod(GetClass()->GetClass(), GetMethod(), false));
	}

	ReturnPointer<JObject> JMethod::CallObjectVariadic(JReference* object, va_list args)
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->CallObjectMethodV(object->GetReference(), GetMethod(), args));
	}

	ReturnPointer<JObject> JMethod::CallObjectArray(JReference* object, jvalue* args)
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->CallObjectMethodA(object->GetReference(), GetMethod(), args));
	}

#define BRICKS_JAVA_METHOD_CALL(T, name) \
	T JMethod::Call##name##Variadic(JReference* object, va_list args) \
	{ \
		return GetVM()->GetEnv()->Call##name##MethodV(object->GetReference(), GetMethod(), args); \
	} \
	\
	T JMethod::Call##name##Array(JReference* object, jvalue* args) \
	{ \
		return GetVM()->GetEnv()->Call##name##MethodA(object->GetReference(), GetMethod(), args); \
	}
		BRICKS_JAVA_TYPES_DEFINITIONS_ALL(BRICKS_JAVA_METHOD_CALL);
#undef BRICKS_JAVA_METHOD_CALL

	JStaticMethod::JStaticMethod(JClass* clazz, jmethodID method) :
		JMethod(clazz, method)
	{

	}

	JStaticMethod::~JStaticMethod()
	{

	}

	ReturnPointer<JObject> JStaticMethod::GetReflection()
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->ToReflectedMethod(GetClass()->GetClass(), GetMethod(), true));
	}

	ReturnPointer<JObject> JStaticMethod::CallObjectVariadic(JReference* object, va_list args)
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->CallStaticObjectMethodV((jclass)object->GetReference(), GetMethod(), args));
	}

	ReturnPointer<JObject> JStaticMethod::CallObjectArray(JReference* object, jvalue* args)
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->CallStaticObjectMethodA((jclass)object->GetReference(), GetMethod(), args));
	}

#define BRICKS_JAVA_METHOD_CALL(T, name) \
	T JStaticMethod::Call##name##Variadic(JReference* object, va_list args) \
	{ \
		return GetVM()->GetEnv()->CallStatic##name##MethodV((jclass)object->GetReference(), GetMethod(), args); \
	} \
	\
	T JStaticMethod::Call##name##Array(JReference* object, jvalue* args) \
	{ \
		return GetVM()->GetEnv()->CallStatic##name##MethodA((jclass)object->GetReference(), GetMethod(), args); \
	}
		BRICKS_JAVA_TYPES_DEFINITIONS_ALL(BRICKS_JAVA_METHOD_CALL);
#undef BRICKS_JAVA_METHOD_CALL

	JNonvirtualMethod::JNonvirtualMethod(JClass* clazz, jmethodID method) :
		JMethod(clazz, method)
	{

	}

	JNonvirtualMethod::JNonvirtualMethod(JMethod* method) :
		JMethod(*method)
	{

	}

	JNonvirtualMethod::~JNonvirtualMethod()
	{

	}

	ReturnPointer<JObject> JNonvirtualMethod::CallObjectVariadic(JReference* object, va_list args)
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->CallNonvirtualObjectMethodV(object->GetReference(), GetClass()->GetClass(), GetMethod(), args));
	}

	ReturnPointer<JObject> JNonvirtualMethod::CallObjectArray(JReference* object, jvalue* args)
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->CallNonvirtualObjectMethodA(object->GetReference(), GetClass()->GetClass(), GetMethod(), args));
	}

#define BRICKS_JAVA_METHOD_CALL(T, name) \
	T JNonvirtualMethod::Call##name##Variadic(JReference* object, va_list args) \
	{ \
		return GetVM()->GetEnv()->CallNonvirtual##name##MethodV(object->GetReference(), GetClass()->GetClass(), GetMethod(), args); \
	} \
	\
	T JNonvirtualMethod::Call##name##Array(JReference* object, jvalue* args) \
	{ \
		return GetVM()->GetEnv()->CallNonvirtual##name##MethodA(object->GetReference(), GetClass()->GetClass(), GetMethod(), args); \
	}
		BRICKS_JAVA_TYPES_DEFINITIONS_ALL(BRICKS_JAVA_METHOD_CALL);
#undef BRICKS_JAVA_METHOD_CALL
} }
