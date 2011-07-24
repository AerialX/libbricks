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
			localStorage = &GlobalAlloc<ThreadLocalStorageDictionary>(AutoPointer< ValueComparison<const void*> >(GlobalAlloc<OperatorValueComparison<const void*> >(), false), AutoPointer< ValueComparison< AutoPointer<Object> > >(GlobalAlloc<OperatorValueComparison< AutoPointer<Object> > >(), false));
		return *localStorage;
#else
		return localStorage.GetItem(GetCurrentThreadID(), TempAlloc<ThreadLocalStorageDictionary>());
#endif
	}

} } }
