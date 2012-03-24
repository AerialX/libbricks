#pragma once

#include "bricks/java/jreference.h"
#include "bricks/java/jvm.h"

#include <jni.h>

namespace Bricks { namespace Java {
	class JClass;

	namespace Internal {
		template<> struct TypeConversion<JObject> {
			typedef ReturnPointer<JObject> Type;
			static Type Convert(const Type& value) { return value; }
			static jobject trevnoC(JReference* value) { return value->GetReference(); }
		};
		class JQualifiedObjectBase { };
	}

	class JObject : public JReference
	{
	protected:
		AutoPointer<JClass> clazz;

		void FillClass();

	public:
		JObject(JClass* clazz, jobject object, bool global = false);
		JObject(JVM* vm, jobject object, bool global = false);
		JObject(jobject object, bool global = false);
		JObject(JReference* reference);
		~JObject();

		jobject GetObject() const { return (jobject)GetReference(); }
		JClass* GetClass() { if (!clazz) FillClass(); return clazz; }

		bool IsTypeOf(JClass* clazz);
	};

#define BRICKS_JAVA_QUALIFIED_OBJECT_START(name, qualified) \
	class name : public Bricks::Java::JQualifiedObject<qualified> \
	{ \
	public: \
		name(JVM* vm, jobject object, bool global = false) : JQualifiedObject(vm, object, global) { BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR(); } \
		name(jobject object, bool global = false) : JQualifiedObject(JVM::GetCurrentVM(), object, global) { BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR(); } \
		name(JVM* vm = JVM::GetCurrentVM()) : JQualifiedObject((autonew Class(vm))->Allocate()) { BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR(); } \
		name(JReference* reference) : JQualifiedObject(reference) { BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR(); } \
		Class* GetClass() { return Bricks::CastTo<Class>(JQualifiedObject<qualified>::GetClass()); } \
	protected: \
		void BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR() { \
			clazz = autonew Class(vm);
#define BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR FillMethods
#define BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR_METHOD(name) BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR_METHOD_NAMED(name, #name)
#define BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR_METHOD_NAMED(name, jname) \
			name.SetObject(this); \
			name.SetName(jname)
#define BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR_END() \
		}
#define BRICKS_JAVA_QUALIFIED_OBJECT_METHOD(T, name) \
	public: \
		Bricks::Java::JBoundMethodDelegate<T> name
#define BRICKS_JAVA_QUALIFIED_OBJECT_STATIC_METHOD(T, name) \
	public: \
		Bricks::Java::JBoundStaticMethodDelegate<T> name
#define BRICKS_JAVA_QUALIFIED_OBJECT_FIELD(T, name) BRICKS_JAVA_QUALIFIED_OBJECT(T, name, #name)
#define BRICKS_JAVA_QUALIFIED_OBJECT_FIELD_NAMED(T, name, jname) \
	protected: \
		Bricks::AutoPointer<Bricks::Java::JField> field##name; \
		void FillField##name() { if (!field##name) field##name = GetClass()->GetField<T>(jname); } \
	public: \
		T GetField##name() { FillField##name(); return field##name->GetValue<T>(this); } \
		void SetField##name(T value) { FillField##name(); field##name->SetValue(this, value); }
#define BRICKS_JAVA_QUALIFIED_OBJECT_END() \
	}
#define BRICKS_JAVA_QUALIFIED_OBJECT(name, qualified) \
	BRICKS_JAVA_QUALIFIED_OBJECT_START(name, qualified) \
		BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR_END() \
	BRICKS_JAVA_QUALIFIED_OBJECT_END();

	template<typename T>
	class JQualifiedObject : public JObject, public Internal::JQualifiedObjectBase
	{
	public:
		typedef T Class;
		typedef const JQualifiedObject<T>& Type;

		JQualifiedObject(JClass* clazz, jobject object, bool global = false) : JObject(clazz, object, global) { }
		JQualifiedObject(JVM* vm, jobject object, bool global = false) : JObject(vm, object, global) { }
		JQualifiedObject(JReference* reference) : JObject(reference) { }
		~JQualifiedObject() { }
	};
} }
