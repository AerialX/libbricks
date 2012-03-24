#pragma once

#include "bricks/core/delegate.h"
#include "bricks/java/jclass.h"
#include "bricks/java/jmethod.h"

namespace Bricks { namespace Collections { template<typename T> class Array; } }

namespace Bricks { namespace Java { namespace Internal {
	static inline void JMethodDelegateValue(jvalue& entry, bool value) { entry.z = value; }
	static inline void JMethodDelegateValue(jvalue& entry, s8 value) { entry.b = value; }
	static inline void JMethodDelegateValue(jvalue& entry, u8 value) { entry.b = value; }
	static inline void JMethodDelegateValue(jvalue& entry, char value) { entry.c = value; }
	static inline void JMethodDelegateValue(jvalue& entry, u16 value) { entry.s = value; }
	static inline void JMethodDelegateValue(jvalue& entry, s16 value) { entry.s = value; }
	static inline void JMethodDelegateValue(jvalue& entry, s32 value) { entry.i = value; }
	static inline void JMethodDelegateValue(jvalue& entry, u32 value) { entry.i = value; }
	static inline void JMethodDelegateValue(jvalue& entry, s64 value) { entry.j = value; }
	static inline void JMethodDelegateValue(jvalue& entry, u64 value) { entry.j = value; }
	static inline void JMethodDelegateValue(jvalue& entry, float value) { entry.f = value; }
	static inline void JMethodDelegateValue(jvalue& entry, double value) { entry.d = value; }
	static inline void JMethodDelegateValue(jvalue& entry, jobject value) { entry.l = value; }
	template<typename T> static inline void JMethodDelegateValue(jvalue& entry, T value) { entry.l = TypeConversion<T>::trevnoC(value); }

	template<typename T, typename E = void> struct JSignature;
	template<typename T> struct JSignature<T, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<JQualifiedClassBase, typename T::Class>::Value && !SFINAE::IsConst<T>::Value>::Type> { static String Signature() { return "L" + T::Class::QualifiedName + ";"; } };
	template<typename T> struct JSignature<const T> { static String Signature() { return JSignature<T>::Signature(); } };
	template<typename T> struct JSignature<T*> { static String Signature() { return JSignature<T>::Signature(); } };
	template<typename T> struct JSignature<T&> { static String Signature() { return JSignature<T>::Signature(); } };
#define BRICKS_JAVA_JMETHODDELEGATESIGNATURE(T, c) \
	template<> struct JSignature<T> { static String Signature() { return c; } };
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(void, 'V');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(bool, 'Z');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(s8, 'B');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(u8, 'B');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(char, 'C');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(u16, 'S');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(s16, 'S');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(u32, 'I');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(s32, 'I');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(u64, 'J');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(s64, 'J');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(float, 'F');
	BRICKS_JAVA_JMETHODDELEGATESIGNATURE(double, 'D');
#undef BRICKS_JAVA_JMETHODDELEGATESIGNATURE
} } }

#define BRICKS_JAVA_JMETHODDELEGATE_VALUE(n, args, arg) Bricks::Java::Internal::JMethodDelegateValue(args[n - 1], arg##n)
#define BRICKS_JAVA_JMETHODDELEGATE_(count, ...) BRICKS_ARGLIST_ENUM_##count(BRICKS_JAVA_JMETHODDELEGATE_VALUE, __VA_ARGS__)
#define BRICKS_JAVA_JMETHODDELEGATE(count, ...) BRICKS_JAVA_JMETHODDELEGATE_(count, __VA_ARGS__)

#define BRICKS_JAVA_JSIGNATURE_VALUE(n, args, arg) signature += Bricks::Java::Internal::JSignature<arg##n>::Signature()
#define BRICKS_JAVA_JSIGNATURE_(count, ...) BRICKS_ARGLIST_ENUM_##count(BRICKS_JAVA_JSIGNATURE_VALUE, __VA_ARGS__)
#define BRICKS_JAVA_JSIGNATURE(count, ...) BRICKS_JAVA_JSIGNATURE_(count, __VA_ARGS__)

#define BRICKS_ARGLIST_HEADER "bricks/java/jmethoddelegate_internal.h"
#include "bricks/core/arglist.h"
#undef BRICKS_ARGLIST_HEADER

#undef BRICKS_JAVA_JMETHODDELEGATE
#undef BRICKS_JAVA_JMETHODDELEGATE_VALUE

namespace Bricks { namespace Java {
	template<typename T> inline ReturnPointer<JField> JClass::GetField(const String& name) { return GetField(name, Internal::JSignature<T>::Signature()); }
	template<typename T> inline ReturnPointer<JStaticField> JClass::GetStaticField(const String& name) { return GetStaticField(name, Internal::JSignature<T>::Signature()); }

	template<typename T> inline JMethodDelegate<T> JClass::GetMethod(const String& name) { return JMethodDelegate<T>(GetMethod(name, Internal::JSignature<T>::Signature())); }
	template<typename T> inline JStaticMethodDelegate<T> JClass::GetStaticMethod(const String& name) { return JStaticMethodDelegate<T>(GetStaticMethod(name, Internal::JSignature<T>::Signature())); }
	template<typename T> inline JMethodDelegate<T> JClass::GetConstructor() { return JMethodDelegate<T>(GetConstructor(Internal::JSignature<T>::Signature())); }
} }
