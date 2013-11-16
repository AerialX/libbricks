#include "bricks/java/jvm.h"
#include "bricks/java/jclass.h"

#include "bricks/core/exception.h"

namespace Bricks { namespace Java {
#if !BRICKS_ENV_ANDROID
	JVM::JVM(JniVersion::Enum version) :
		owner(true)
	{
		JNIEnv* env;

		JavaVMInitArgs args;
		args.version = version;
		args.nOptions = 0;
		JNI_GetDefaultJavaVMInitArgs(&args);
		JNI_CreateJavaVM(&vm, (void**)&env, &args);
		environments.SetValue(env);
	}
#endif

	JVM::JVM(JavaVM* vm) :
		owner(false), vm(vm)
	{

	}

	JVM::~JVM()
	{
		if (owner)
			vm->DestroyJavaVM();
	}

	JNIEnv* JVM::GetEnv()
	{
		if (environments.HasValue())
			return environments.GetValue();

		JNIEnv* env;
#if BRICKS_ENV_ANDROID
		if (vm->AttachCurrentThread(&env, NULL))
#else
		if (vm->AttachCurrentThread((void**)&env, NULL))
#endif
			BRICKS_FEATURE_THROW(Exception());
		environments.SetValue(env);

		return env;
	}

	ReturnPointer<JClass> JVM::GetClass(const String& name)
	{
		jclass clz = GetEnv()->FindClass(name.CString());
		if (!clz)
			return NULL;
		return autonew JClass(this, clz);
	}

	static AutoPointer<JVM> defaultVM;

	void JVM::SetCurrentVM(JVM* vm)
	{
		defaultVM = vm;
	}

	ReturnPointer<JVM> JVM::GetCurrentVM()
	{
		if (defaultVM)
			return defaultVM;
#if BRICKS_ENV_ANDROID
		return NULL;
#else
		jsize vmCount = 0;
		JavaVM* vms;
		JNI_GetCreatedJavaVMs(&vms, 1, &vmCount);
		if (!vmCount)
			return NULL;
		return autonew JVM(vms);
#endif
	}
} }
