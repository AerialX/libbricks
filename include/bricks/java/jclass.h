#pragma once

#include "bricks/core/string.h"
#include "bricks/java/jreference.h"
#include "bricks/java/jvm.h"

#include <jni.h>

namespace Bricks { template<typename T> class ReturnPointer; }

namespace Bricks { namespace Java {
	class JObject;
	class JField;
	class JMethod;
	class JStaticField;
	class JStaticMethod;
	class JNonvirtualMethod;

	template<typename T> class JMethodDelegate;
	template<typename T> class JBoundMethodDelegate;
	template<typename T> class JStaticMethodDelegate;
	namespace Internal {
		template<typename T> struct JConstructorMethodDelegateTransform;
		class JQualifiedClassBase { };
	}

	class JClass : public JReference
	{
	public:
		JClass(JVM* vm, jclass clazz, bool global = false);
		JClass(JVM* vm, jobject object);
		JClass(JReference* reference);
		~JClass();

		jclass GetClass() const { return (jclass)GetReference(); }

		ReturnPointer<JClass> GetSuperclass();

		ReturnPointer<JObject> Allocate();

		ReturnPointer<JObject>::Type Construct(...);

		ReturnPointer<JField> GetField(const String& name, const String& signature);
		ReturnPointer<JStaticField> GetStaticField(const String& name, const String& signature);

		ReturnPointer<JMethod> GetMethod(const String& name, const String& signature);
		ReturnPointer<JStaticMethod> GetStaticMethod(const String& name, const String& signature);
		ReturnPointer<JNonvirtualMethod> GetConstructor(const String& signature);

		template<typename T> ReturnPointer<T> Allocate() { return Internal::TypeConversion<T>::Convert(Allocate()); }

		template<typename T> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct();
		template<typename T, typename T1> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1);
		template<typename T, typename T1, typename T2> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1, T2 arg2);
		template<typename T, typename T1, typename T2, typename T3> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1, T2 arg2, T3 arg3);
		template<typename T, typename T1, typename T2, typename T3, typename T4> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1, T2 arg2, T3 arg3, T4 arg4);
		template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5);
		template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6);
		template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7);
		template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8);
		template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9> typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type Construct(T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9);

		template<typename T> ReturnPointer<JField> GetField(const String& name);
		template<typename T> ReturnPointer<JStaticField> GetStaticField(const String& name);

		template<typename T> JMethodDelegate<T> GetMethod(const String& name);
		template<typename T> JStaticMethodDelegate<T> GetStaticMethod(const String& name);
		template<typename T> JMethodDelegate<T> GetConstructor();
	};

#define BRICKS_JAVA_QUALIFIED_CLASS_START(name, qualified) \
	Bricks::String __BricksJavaQualifiedName##name(qualified); \
	class name : public Bricks::Java::JQualifiedClass<__BricksJavaQualifiedName##name> \
	{ \
	public: \
		name(JVM* vm, jclass clazz, bool global = false) : JQualifiedClass(vm, clazz, global) { BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR(); } \
		name(JReference* reference) : JQualifiedClass(reference) { BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR(); } \
		name(JVM* vm = JVM::GetCurrentVM()) : JQualifiedClass(vm) { BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR(); } \
	protected: \
		void BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR() {
#define BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR FillMethods
#define BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR_METHOD(name) BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR_METHOD_NAMED(name, #name)
#define BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR_METHOD_NAMED(name, jname) \
			name.SetClass(this); \
			name.SetName(jname)
#define BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR_END() \
		}
#define BRICKS_JAVA_QUALIFIED_CLASS_METHOD(T, name) \
	public: \
		Bricks::Java::JBoundStaticMethodDelegate<T> name
#define BRICKS_JAVA_QUALIFIED_CLASS_FIELD(T, name) BRICKS_JAVA_QUALIFIED_CLASS_FIELD_NAMED(T, name, #name)
#define BRICKS_JAVA_QUALIFIED_CLASS_FIELD_NAMED(T, name, jname) \
	protected: \
		Bricks::AutoPointer<Bricks::Java::JStaticField> field##name; \
		void FillField##name() { if (!field##name) field##name = GetStaticField<T>(jname); } \
	public: \
		T GetField##name() { FillField##name(); return field##name->GetValue<T>(); } \
		void SetField##name(T value) { FillField##name(); field##name->SetValue(value); }
#define BRICKS_JAVA_QUALIFIED_CLASS_END() \
	}
#define BRICKS_JAVA_QUALIFIED_CLASS(name, qualified) \
	BRICKS_JAVA_QUALIFIED_CLASS_START(name, qualified) \
		BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR_END() \
	BRICKS_JAVA_QUALIFIED_CLASS_END()

	template<const String& N>
	class JQualifiedClass : public JClass, public Internal::JQualifiedClassBase
	{
	public:
		static const String& QualifiedName;

		JQualifiedClass(JVM* vm = JVM::GetCurrentVM()) : JClass(vm->GetClass(QualifiedName)) { }
		JQualifiedClass(JVM* vm, jclass clazz, bool global = false) : JClass(vm, clazz, global) { }
		JQualifiedClass(JVM* vm, jobject object, bool global = false) : JClass(vm, object, global) { }
		JQualifiedClass(JReference* reference) : JClass(reference) { }
		~JQualifiedClass() { }
	};

	template<const String& N> const String& JQualifiedClass<N>::QualifiedName(N);
} }
