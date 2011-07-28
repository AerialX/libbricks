#include "bricksall.hpp"

using namespace Bricks::Collections;

namespace Bricks { namespace Threading { namespace Internal {
#ifdef BRICKS_FEATURE_TLS
	static BRICKS_FEATURE_TLS ThreadLocalStorageDictionary* localStorage = NULL;
#else
	static Dictionary<ThreadID, AutoPointer<ThreadLocalStorageDictionary> > localStorage;
#endif

	static void ThreadLocalStorageDestructor()
	{
#ifdef BRICKS_FEATURE_TLS
		if (localStorage)
			delete localStorage;
#else
		localStorage.RemoveKey(GetCurrentThreadID());
#endif
	}

	ThreadLocalStorageDictionary& GetStorageDictionary()
	{
#ifdef BRICKS_FEATURE_TLS
		if (!localStorage)
			localStorage = new (Bricks::Internal::Global)ThreadLocalStorageDictionary(autonew (Bricks::Internal::Global)OperatorValueComparison<const void*>(), autonew (Bricks::Internal::Global)OperatorValueComparison< AutoPointer<Object> >());
		return *localStorage;
#else
		return localStorage.GetItem(GetCurrentThreadID(), autonew ThreadLocalStorageDictionary());
#endif
	}

} } }
