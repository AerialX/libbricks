#include "bricks/core/object.h"
#include "bricks/core/typeinfo.h"
#include "bricks/core/exception.h"

#if BRICKS_CONFIG_LOGGING_MEMLEAK
#include "bricks/collections/array.h"

using namespace Bricks::Collections;
#endif

namespace Bricks {
#if BRICKS_CONFIG_RTTI
	String Object::GetDebugString() const
	{
		return String::Format("%s <%p> [%d]", TypeOf(this).GetName().CString(), this, GetReferenceCount());
	}
#else
	String Object::GetDebugString() const
	{
		return String::Format("<%p> [%d]", this, GetReferenceCount());
	}
#endif

#if BRICKS_ENV_DEBUG
	void Object::Retain()
	{
		referenceCount++;
		BRICKS_FEATURE_LOG_HEAVY("+ %p [%d]", this, GetReferenceCount());
	}

	void Object::Release()
	{
		BRICKS_FEATURE_LOG_HEAVY("- %p [%d]", this, GetReferenceCount() - 1);
		BRICKS_FEATURE_ASSERT(referenceCount > 0);
		if (!--referenceCount)
			delete this;
	}
#endif
}

#if !BRICKS_ENV_APPLE && BRICKS_ENV_DEBUG
extern "C" __attribute__((weak)) const char* _NSPrintForDebugger(Bricks::Object* object)
{
	return object->GetDebugString().CString();
}
#endif
