#include "bricks/threading.h"

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
			localStorage = &globalalloc ThreadLocalStorageDictionary(AutoPointer< ValueComparison<const void*> >(globalalloc OperatorValueComparison<const void*>(), false), AutoPointer< ValueComparison< AutoPointer<Object> > >(globalalloc OperatorEqualityComparison< AutoPointer<Object> >(), false));
		return *localStorage;
#else
		return localStorage.GetItem(GetCurrentThreadID(), AutoPointer<ThreadLocalStorageDictionary>(alloc ThreadLocalStorageDictionary(), false));
#endif
	}

} } }
