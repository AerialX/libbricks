#include "bricks/java/jreference.h"
#include "bricks/java/jvm.h"

namespace Bricks { namespace Java {
	JReference::JReference(JVM* vm, jobject reference, bool global) :
		vm(vm), reference(reference), global(global)
	{

	}

	JReference::JReference(const JReference& instance) :
		vm(instance.vm), reference(instance.reference), global(false)
	{
		if (instance.global)
			MakeGlobal();
	}

	JReference::~JReference()
	{
		if (global)
			vm->GetEnv()->DeleteGlobalRef(reference);
	}

	void JReference::MakeGlobal()
	{
		if (!global) {
			jobject ref = vm->GetEnv()->NewGlobalRef(reference);
			if (!ref)
				BRICKS_FEATURE_THROW(OutOfMemoryException());
			reference = ref;
			global = true;
		}
	}
} }
