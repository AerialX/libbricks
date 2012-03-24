#pragma once

#include "bricks/java/jobject.h"
#include "bricks/java/jmethoddelegate.h"

namespace Bricks { namespace Java { namespace Lang {
	BRICKS_JAVA_QUALIFIED_CLASS_START(StringClass, "java/lang/String");
		BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR_END();
	BRICKS_JAVA_QUALIFIED_CLASS_END();
	BRICKS_JAVA_QUALIFIED_OBJECT_START(String, StringClass);
			BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR_METHOD(length);
		BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR_END();
		BRICKS_JAVA_QUALIFIED_OBJECT_METHOD(int(), length);
	protected:
		void CreateString(const Bricks::String& string)
		{
			clazz = autonew Class(vm);
			reference = vm->GetEnv()->NewStringUTF(string.CString());

			BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR();
		}

	public:
		String(const Bricks::String& string) :
			JQualifiedObject(JVM::GetCurrentVM(), NULL)
		{
			CreateString(string);
		}

		String(const char* string) :
			JQualifiedObject(JVM::GetCurrentVM(), NULL)
		{
			CreateString(string);
		}

		String(JVM* vm, const Bricks::String& string) :
			JQualifiedObject(vm, NULL)
		{
			CreateString(string);
		}

		jstring GetJString() const { return (jstring)GetObject(); }

		Bricks::String GetString()
		{
			const char* ptr = vm->GetEnv()->GetStringUTFChars(GetJString(), NULL);
			Bricks::String ret(ptr);
			vm->GetEnv()->ReleaseStringUTFChars(GetJString(), ptr);
			return ret;
		}
	BRICKS_JAVA_QUALIFIED_OBJECT_END();
} } }

namespace Bricks { namespace Java { namespace Internal {
	template<> struct JSignature<String> { static String Signature() { return JSignature<Lang::String>::Signature(); } };
	template<> struct TypeConversion<String>
	{
		typedef String Type;
		static Type Convert(JReference* value) { return Lang::String(value).GetString(); }
		static jobject trevnoC(const String& value) { return Lang::String(value).GetReference(); }
	};
} } }
