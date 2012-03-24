#pragma once

#include "bricks/java/jclass.h"

#include <jni.h>

namespace Bricks { namespace Java {
	class JMethod : public Object
	{
	protected:
		JClass* clazz;
		jmethodID method;

		virtual ReturnPointer<JObject> CallObjectVariadic(JReference* object, va_list args);
		virtual ReturnPointer<JObject> CallObjectArray(JReference* object, jvalue* args);
#define BRICKS_JAVA_METHOD_CALL(T, name) \
		virtual T Call##name##Variadic(JReference* object, va_list args); \
		virtual T Call##name##Array(JReference* object, jvalue* args);
		BRICKS_JAVA_TYPES_DEFINITIONS_ALL(BRICKS_JAVA_METHOD_CALL);
#undef BRICKS_JAVA_METHOD_CALL

	public:
		JMethod(JClass* clazz, jmethodID method);
		~JMethod();

		jmethodID GetMethod() const { return method; }
		JClass* GetClass() const { return clazz; }
		JVM* GetVM() const { return clazz->GetVM(); }

		ReturnPointer<JObject> GetReflection();

		template<typename T> typename Internal::TypeConversion<T>::Type Call(JReference* object, ...);
		template<typename T> typename Internal::TypeConversion<T>::Type CallVariadic(JReference* object, va_list args) { return Internal::TypeConversion<T>::Convert(CallObjectVariadic(object, args)); }
		template<typename T> typename Internal::TypeConversion<T>::Type CallArray(JReference* object, jvalue* args) { return Internal::TypeConversion<T>::Convert(CallObjectArray(object, args)); }
	};
#define BRICKS_JAVA_METHOD_CALL_(T, name) \
	template<> inline T JMethod::CallVariadic<T>(JReference* object, va_list args) { return Call##name##Variadic(object, args); } \
	template<> inline T JMethod::CallArray<T>(JReference* object, jvalue* args) { return Call##name##Array(object, args); }
#define BRICKS_JAVA_METHOD_CALL(T, name) \
	template<> inline T JMethod::Call<T>(JReference* object, ...) { va_list args; va_start(args, object); T ret = Call##name##Variadic(object, args); va_end(args); return ret; } \
	BRICKS_JAVA_METHOD_CALL_(T, name)
	BRICKS_JAVA_TYPES(BRICKS_JAVA_METHOD_CALL);
#undef BRICKS_JAVA_METHOD_CALL
#define BRICKS_JAVA_METHOD_CALL(T, name) \
	template<> inline T JMethod::Call<T>(JReference* object, ...) { va_list args; va_start(args, object); Call##name##Variadic(object, args); va_end(args); } \
	BRICKS_JAVA_METHOD_CALL_(T, name)
	BRICKS_JAVA_TYPES_VOID(BRICKS_JAVA_METHOD_CALL);
#undef BRICKS_JAVA_METHOD_CALL
#undef BRICKS_JAVA_METHOD_CALL_

	class JStaticMethod : public JMethod
	{
	protected:
		ReturnPointer<JObject> CallObjectVariadic(JReference* object, va_list args);
		ReturnPointer<JObject> CallObjectArray(JReference* object, jvalue* args);
#define BRICKS_JAVA_METHOD_CALL(T, name) \
		T Call##name##Variadic(JReference* object, va_list args); \
		T Call##name##Array(JReference* object, jvalue* args);
		BRICKS_JAVA_TYPES_DEFINITIONS_ALL(BRICKS_JAVA_METHOD_CALL);
#undef BRICKS_JAVA_METHOD_CALL

	public:
		JStaticMethod(JClass* clazz, jmethodID method);
		~JStaticMethod();

		ReturnPointer<JObject> GetReflection();

		template<typename T> typename Internal::TypeConversion<T>::Type CallVariadic(va_list args) { return JMethod::CallVariadic<T>(GetClass(), args); }
		template<typename T> typename Internal::TypeConversion<T>::Type CallArray(jvalue* args) { return JMethod::CallArray<T>(GetClass(), args); }
	};

	class JNonvirtualMethod : public JMethod
	{
	protected:
		ReturnPointer<JObject> CallObjectVariadic(JReference* object, va_list args);
		ReturnPointer<JObject> CallObjectArray(JReference* object, jvalue* args);
#define BRICKS_JAVA_METHOD_CALL(T, name) \
		T Call##name##Variadic(JReference* object, va_list args); \
		T Call##name##Array(JReference* object, jvalue* args);
		BRICKS_JAVA_TYPES_DEFINITIONS_ALL(BRICKS_JAVA_METHOD_CALL);
#undef BRICKS_JAVA_METHOD_CALL

	public:
		JNonvirtualMethod(JClass* clazz, jmethodID method);
		JNonvirtualMethod(JMethod* method);
		~JNonvirtualMethod();
	};
} }
