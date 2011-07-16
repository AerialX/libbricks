#ifndef BRICKS_ARGLIST_COUNT
#error Use object.h
#endif

namespace Bricks {
	template<typename T BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES> static inline T& Alloc(BRICKS_ARGLIST_TYPES_NAMES) { return *new T(BRICKS_ARGLIST_ARGS); }
	template<typename T BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES> static inline T& GlobalAlloc(BRICKS_ARGLIST_TYPES_NAMES) { return *new (Bricks::Internal::Global) T(BRICKS_ARGLIST_ARGS); }
	template<typename T BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES> static inline T& AutoAlloc(BRICKS_ARGLIST_TYPES_NAMES) { T* ret = new T(BRICKS_ARGLIST_ARGS); ret->Autorelease(); return *ret; }
	template<typename T BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES> static inline AutoPointer<T> TempAlloc(BRICKS_ARGLIST_TYPES_NAMES) { return AutoPointer<T>(new T(BRICKS_ARGLIST_ARGS), false); }
}
