#pragma once

#include "bricks/core/returnpointer.h"
#include "bricks/java/jclass.h"
#include "bricks/java/jobject.h"

#include <jni.h>

namespace Bricks { namespace Java {
	class JField : public Object
	{
	protected:
		JClass* clazz;
		jfieldID field;

		ReturnPointer<JObject> GetObjectValue(JReference* object);
		void SetObjectValue(JReference* object, JReference* value);
#define BRICKS_JAVA_FIELD_GETSET(T, name) \
		virtual T Get##name##Value(JReference* object); \
		virtual void Set##name##Value(JReference* object, T value)
		BRICKS_JAVA_TYPES_DEFINITIONS(BRICKS_JAVA_FIELD_GETSET);
#undef BRICKS_JAVA_FIELD_GETSET

	public:
		JField(JClass* clazz, jfieldID field);
		~JField();

		jfieldID GetField() const { return field; }
		JClass* GetClass() const { return clazz; }
		JVM* GetVM() const { return clazz->GetVM(); }

		ReturnPointer<JObject> GetReflection();

		template<typename T> typename Internal::TypeConversion<T>::Type GetValue(JReference* object) { return Internal::TypeConversion<T>::Convert(GetObjectValue(object)); }

		void SetValue(JReference* object, JReference* value) { SetObjectValue(object, value); }
#define BRICKS_JAVA_FIELD_SETVALUE(T, name) \
		void SetValue(JReference* object, T value) { Set##name##Value(object, value); }
		BRICKS_JAVA_TYPES(BRICKS_JAVA_FIELD_SETVALUE);
#undef BRICKS_JAVA_FIELD_SETVALUE
	};

#define BRICKS_JAVA_FIELD_GETVALUE(T, name) \
	template<> inline T JField::GetValue<T>(JReference* object) { return Get##name##Value(object); }
	BRICKS_JAVA_TYPES(BRICKS_JAVA_FIELD_GETVALUE);
#undef BRICKS_JAVA_FIELD_GETVALUE

	class JStaticField : public JField
	{
	protected:
		ReturnPointer<JObject> GetObjectValue(JReference* object);
		void SetObjectValue(JReference* object, JReference* value);
#define BRICKS_JAVA_FIELD_GETSET(T, name) \
		T Get##name##Value(JReference* object); \
		void Set##name##Value(JReference* object, T value)
		BRICKS_JAVA_TYPES_DEFINITIONS(BRICKS_JAVA_FIELD_GETSET);
#undef BRICKS_JAVA_FIELD_GETSET

	public:
		JStaticField(JClass* clazz, jfieldID method);
		~JStaticField();

		ReturnPointer<JObject> GetReflection();

		template<typename T> void SetValue(T value) { JField::SetValue(GetClass(), value); }
		template<typename T> typename Internal::TypeConversion<T>::Type GetValue() { return JField::GetValue<T>(GetClass()); }
	};
} }
