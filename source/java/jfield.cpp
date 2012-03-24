#include "bricks/java/jfield.h"
#include "bricks/java/jclass.h"
#include "bricks/java/jobject.h"
#include "bricks/java/jvm.h"

namespace Bricks { namespace Java {
	JField::JField(JClass* clazz, jfieldID field) :
		clazz(clazz), field(field)
	{

	}

	JField::~JField()
	{

	}

	ReturnPointer<JObject> JField::GetReflection()
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->ToReflectedField(GetClass()->GetClass(), GetField(), false));
	}

	ReturnPointer<JObject> JField::GetObjectValue(JReference* object)
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->GetObjectField(object->GetReference(), GetField()));
	}

	void JField::SetObjectValue(JReference* object, JReference* value)
	{
		GetVM()->GetEnv()->SetObjectField(object->GetReference(), GetField(), value->GetReference());
	}

#define BRICKS_JAVA_FIELD_GETSET(T, name) \
	T JField::Get##name##Value(JReference* object) \
	{ \
		return GetVM()->GetEnv()->Get##name##Field(object->GetReference(), GetField()); \
	} \
	\
	void JField::Set##name##Value(JReference* object, T value) \
	{ \
		GetVM()->GetEnv()->Set##name##Field(object->GetReference(), GetField(), value); \
	}
	BRICKS_JAVA_TYPES_DEFINITIONS(BRICKS_JAVA_FIELD_GETSET);
#undef BRICKS_JAVA_FIELD_GETSET

	JStaticField::JStaticField(JClass* clazz, jfieldID method) :
		JField(clazz, method)
	{

	}

	JStaticField::~JStaticField()
	{

	}

	ReturnPointer<JObject> JStaticField::GetReflection()
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->ToReflectedField(GetClass()->GetClass(), GetField(), true));
	}

	ReturnPointer<JObject> JStaticField::GetObjectValue(JReference* object)
	{
		return autonew JObject(GetVM(), GetVM()->GetEnv()->GetStaticObjectField((jclass)object->GetReference(), GetField()));
	}

	void JStaticField::SetObjectValue(JReference* object, JReference* value)
	{
		GetVM()->GetEnv()->SetStaticObjectField((jclass)object->GetReference(), GetField(), value->GetReference());
	}

#define BRICKS_JAVA_FIELD_GETSET(T, name) \
	T JStaticField::Get##name##Value(JReference* object) \
	{ \
		return GetVM()->GetEnv()->GetStatic##name##Field((jclass)object->GetReference(), GetField()); \
	} \
	\
	void JStaticField::Set##name##Value(JReference* object, T value) \
	{ \
		GetVM()->GetEnv()->SetStatic##name##Field((jclass)object->GetReference(), GetField(), value); \
	}
	BRICKS_JAVA_TYPES_DEFINITIONS(BRICKS_JAVA_FIELD_GETSET);
#undef BRICKS_JAVA_FIELD_GETSET
} }
