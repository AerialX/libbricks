#pragma once

#include "bricks/core/autopointer.h"
#include "bricks/core/returnpointer.h"

#include <jni.h>

#define BRICKS_JAVA_TYPES_ALL(macro) \
	BRICKS_JAVA_TYPES(macro); \
	BRICKS_JAVA_TYPES_VOID(macro)
#define BRICKS_JAVA_TYPES_VOID(macro) \
	macro(void, Void)
#define BRICKS_JAVA_TYPES(macro) \
	macro(bool, Boolean); \
	macro(u8, Byte); macro(s8, Byte); \
	macro(char, Char); \
	macro(s16, Short); macro(u16, Short); \
	macro(s32, Int); macro(u32, Int); \
	macro(s64, Long); macro(u64, Long); \
	macro(float, Float); \
	macro(double, Double)

#define BRICKS_JAVA_TYPES_CONVERSION(macro) \
	macro(bool, jboolean); \
	macro(u8, jbyte); macro(s8, jbyte); \
	macro(char, jchar); \
	macro(s16, jshort); macro(u16, jshort); \
	macro(s32, jint); macro(u32, jint); \
	macro(s64, jlong); macro(u64, jlong); \
	macro(float, jfloat); \
	macro(double, jdouble)

#define BRICKS_JAVA_TYPES_DEFINITIONS_ALL(macro) \
	BRICKS_JAVA_TYPES_DEFINITIONS(macro); \
	BRICKS_JAVA_TYPES_DEFINITIONS_VOID(macro)
#define BRICKS_JAVA_TYPES_DEFINITIONS_VOID(macro) \
	macro(void, Void)
#define BRICKS_JAVA_TYPES_DEFINITIONS(macro) \
	macro(jboolean, Boolean); \
	macro(jbyte, Byte); \
	macro(jchar, Char); \
	macro(jshort, Short); \
	macro(jint, Int); \
	macro(jlong, Long); \
	macro(jfloat, Float); \
	macro(jdouble, Double)

namespace Bricks { namespace Java {
	class JObject;
	class JVM;

	class JReference : public Object
	{
	protected:
		AutoPointer<JVM> vm;
		jobject reference;
		bool global;

	public:
		JReference(JVM* vm, jobject reference, bool global = false);
		JReference(const JReference& reference);
		~JReference();

		JVM* GetVM() const { return vm; }

		jobject GetReference() const { return reference; }
		bool IsGlobal() const { return global; }

		void MakeGlobal();
	};

	namespace Internal {
		template<typename T, typename E = void> struct TypeConversion { typedef T Type; };
		template<typename T> struct TypeConversion<T, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<JReference, T>::Value && !SFINAE::IsConst<T>::Value>::Type>
		{
			typedef ReturnPointer<T> Type;
			static Type Convert(const ReturnPointer<JObject>& value) { return autonew T(value); }
			static jobject trevnoC(JReference* value) { return value->GetReference(); }
			static jobject trevnoC(const JReference& value) { return value.GetReference(); }
		};

		template<> struct TypeConversion<JReference>
		{
			typedef ReturnPointer<JReference> Type;
			static Type Convert(const Type& value) { return value; }
			static jobject trevnoC(JReference* value) { return value->GetReference(); }
			static jobject trevnoC(const JReference& value) { return value.GetReference(); }
		};

		#define BRICKS_JAVA_TYPECONVERSION(TT, T) \
			template<typename T> struct TypeConversion<TT> { \
				typedef typename TypeConversion<T>::Type Type; \
				template<typename U> static Type Convert(const U& value) { return TypeConversion<T>::Convert(value); } \
				template<typename U> static jobject trevnoC(const U& value) { return TypeConversion<T>::trevnoC(value); } \
			}
		BRICKS_JAVA_TYPECONVERSION(T*, T);
		BRICKS_JAVA_TYPECONVERSION(T&, T);
		BRICKS_JAVA_TYPECONVERSION(const T, T);
		#undef BRICKS_JAVA_TYPECONVERSION
	}
} }
