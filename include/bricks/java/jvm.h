#pragma once

#include "bricks/core/returnpointer.h"
#include "bricks/core/string.h"
#include "bricks/core/value.h"
#include "bricks/threading/threadlocalstorage.h"

#include <jni.h>

namespace Bricks { namespace Java {
	class JClass;

	namespace JniVersion {
		enum Enum {
			Unknown	= 0,
			JNI1_1	= JNI_VERSION_1_1,
			JNI1_2	= JNI_VERSION_1_2,
			JNI1_4	= JNI_VERSION_1_4,
			JNI1_6	= JNI_VERSION_1_6
		};
	}

	class JVM : public Object
	{
	protected:
		bool owner;
		JavaVM* vm;

		Threading::ThreadLocalStorage<JNIEnv*> environments;

	public:
#if !BRICKS_ENV_ANDROID
		JVM(JniVersion::Enum version = JniVersion::JNI1_4);
#endif
		JVM(JavaVM* vm);
		~JVM();

		JNIEnv* GetEnv();

		ReturnPointer<JClass> GetClass(const String& name);

		static void SetCurrentVM(JVM* vm);
		static ReturnPointer<JVM> GetCurrentVM();
	};
} }
