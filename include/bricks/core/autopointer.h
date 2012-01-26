#pragma once

#include "bricks/core/object.h"
#include "bricks/core/pointer.h"

#define autonew Bricks::Internal::AutoAllocPointer() * new

namespace Bricks {
	template<typename T = Object> class AutoPointer : public Pointer<T>
	{
	public:
		AutoPointer() { }
		AutoPointer(const AutoPointer<T>& t, bool retain = true) : Pointer<T>(t) { if (retain) Retain(*this); }
		AutoPointer(const Pointer<T>& t, bool retain = true) : Pointer<T>(t) { if (retain) Retain(*this); }
		AutoPointer(T* t, bool retain = true) : Pointer<T>(t) { if (retain) Retain(*this); }
		template<typename U> AutoPointer(const Pointer<U>& t, bool retain = true) : Pointer<T>(t) { if (retain) Retain(*this);  }

		~AutoPointer() { Release(); }

		AutoPointer<T>& operator=(const Pointer<T>& t) { Swap(t); return *this; }
		AutoPointer<T>& operator=(const AutoPointer<T>& t) { Swap(t); return *this; }
		AutoPointer<T>& operator=(T* t) { Swap(t); return *this; }
		template<typename U> AutoPointer<T>& operator=(const Pointer<U>& t) { Swap(t); return *this; }

		void Swap(const Pointer<T>& t, bool retain = true) { if (this->GetValue() == t.GetValue()) return; Release(*this); Pointer<T>::Swap(t); if (retain) Retain(*this); }
		void Release() { Release(*this); Pointer<T>::Swap(NULL); }

		template<typename U> static void Retain(U* ptr) { if (ptr) CastTo<Object>(ptr)->Retain(); }
		template<typename U> static void Release(U* ptr) { if (ptr) CastTo<Object>(ptr)->Release(); }
		template<typename U> static void Retain(const Pointer<U>& ptr) { Retain(ptr.GetValue()); }
		template<typename U> static void Release(const Pointer<U>& ptr) { Release(ptr.GetValue()); }
	};

	namespace Internal {
		struct AutoAllocPointer
		{
			template<typename T> typename SFINAE::EnableIf<SFINAE::IsCompatibleType<Object, T>::Value, AutoPointer<T> >::Type operator *(T* t) { return AutoPointer<T>(t, false); }
		};
	}
}
