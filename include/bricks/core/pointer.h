#pragma once

#include "bricks/config.h"
#include "bricks/core/sfinae.h"

#define tempnew Bricks::Internal::TemporaryPointer() *

namespace Bricks {
#ifdef BRICKS_CONFIG_RTTI
	template<typename T, typename U> static inline typename SFINAE::EnableIf<SFINAE::IsCompatibleType<T, U>::Value, T*>::Type CastToDynamic(U* u) { return static_cast<T*>(u); }
	template<typename T, typename U> static inline typename SFINAE::EnableIf<!SFINAE::IsCompatibleType<T, U>::Value, T*>::Type CastToDynamic(U* u) { return dynamic_cast<T*>(u); }
	template<typename T, typename U> static inline bool IsTypeOf(U* u) { return dynamic_cast<T*>(u); }

	// CastTo<>() uses static_cast where possible, and dynamic_cast otherwise.
	// In debug mode it also prefers dynamic_cast for base -> derived, while static_cast is used in release mode.
#ifdef BRICKS_FEATURE_RELEASE
	template<typename T, typename U> static inline typename SFINAE::EnableIf<SFINAE::IsCompatibleType<T, U>::Value || SFINAE::IsCompatibleType<U, T>::Value, T*>::Type CastTo(U* u) { return static_cast<T*>(u); }
	template<typename T, typename U> static inline typename SFINAE::EnableIf<!SFINAE::IsCompatibleType<T, U>::Value && !SFINAE::IsCompatibleType<U, T>::Value, T*>::Type CastTo(U* u) { return dynamic_cast<T*>(u); }
#else
	template<typename T, typename U> static inline T* CastTo(U* u) { return CastToDynamic<T, U>(u); }
#endif
#else
	template<typename T, typename U> static inline T* CastTo(U* u) { return (T*)u; }
	template<typename T, typename U> static inline T* CastToDynamic(U* u) { return NULL; }
	template<typename T, typename U> static inline bool IsTypeOf(U* u) { return false; }
#endif
	template<typename T> static inline const T* CastToRaw(const void* u) { return reinterpret_cast<const T*>(u); }
	template<typename T> static inline T* CastToRaw(void* u) { return reinterpret_cast<T*>(u); }
	template<typename U> static inline void* CastToRaw(U* u) { return reinterpret_cast<void*>(u); }

	template<typename T> class Pointer
	{
	private:
		T* value;
	
	public:
		typedef T Type;

		static Pointer<T> Null;

		Pointer() : value(NULL) { }
		Pointer(const Pointer<T>& t) : value(t.value) { }
		Pointer(T* t) : value(t) { }
		template<typename U> Pointer(const Pointer<U>& t, bool retain = true) : value(t.GetValue()) { }

		Pointer<T>& operator=(const Pointer<T>& t) { Swap(t); return *this; }
		Pointer<T>& operator=(T* t) { value = t; return *this; }
		template<typename U> Pointer<T>& operator=(const Pointer<U>& t) { Swap(t); return *this; }

		T* operator->() const { return value; }
		T& operator*() const { return *value; }
		T* GetValue() const { return value; }
		operator T*() const { return value; }

		void Swap(const Pointer<T>& t) { value = t.value; }
	};
	template<typename T> Pointer<T> Pointer<T>::Null = Pointer<T>(NULL);

#define BRICKS_INTERNAL_POINTER_COMPARISON(oper) \
	template<typename T, typename T2> static bool operator oper(const Pointer<T>& t1, const Pointer<T2>& t2) { return t1.GetValue() oper t2.GetValue(); } \
	template<typename T, typename T2> static bool operator oper(T* t1, const Pointer<T2>& t2) { return t1 oper t2.GetValue(); } \
	template<typename T, typename T2> static bool operator oper(const Pointer<T>& t1, T2* t2) { return t1.GetValue() oper t2; }
	BRICKS_INTERNAL_POINTER_COMPARISON(==);
	BRICKS_INTERNAL_POINTER_COMPARISON(!=);
	BRICKS_INTERNAL_POINTER_COMPARISON(>);
	BRICKS_INTERNAL_POINTER_COMPARISON(<);
	BRICKS_INTERNAL_POINTER_COMPARISON(>=);
	BRICKS_INTERNAL_POINTER_COMPARISON(<=);
#undef BRICKS_INTERNAL_POINTER_COMPARISON

	template<typename T, typename U> static inline T* CastTo(const Pointer<U>& u) { return CastTo<T, U>(u.GetValue()); } \
	template<typename T, typename U> static inline T* CastToDynamic(const Pointer<U>& u) { return CastToDynamic<T, U>(u.GetValue()); } \
	template<typename T, typename U> static inline bool IsTypeOf(const Pointer<U>& u) { return IsTypeOf<T, U>(u.GetValue()); } \
	template<typename T, typename U> static inline T* CastToRaw(const Pointer<U>& u) { return CastToRaw<T>(u.GetValue()); } \
	template<typename U> static inline void* CastToRaw(const Pointer<U>& u) { return CastToRaw<U>(u.GetValue()); }

	namespace Internal {
		struct TemporaryPointer
		{
			template<typename T> T* operator *(const T& t) { return const_cast<T*>(&t); }
		};
	}
}
