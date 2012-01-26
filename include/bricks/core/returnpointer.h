#pragma once

#include "bricks/core/autopointer.h"

namespace Bricks {
	template<typename T = Object> class ReturnPointer : public AutoPointer<T>
	{
	private:
		bool retained;

		template<typename U> friend class ReturnPointer;

	public:
		ReturnPointer() : retained(false) { }
		ReturnPointer(T* t, bool retain = false) : AutoPointer<T>(t, retain), retained(retain) { }
		ReturnPointer(const Pointer<T>& t, bool retain = false) : AutoPointer<T>(t, retain), retained(retain) { }
		ReturnPointer(const AutoPointer<T>& t, bool retain = true) : AutoPointer<T>(t, retain), retained(retain) { }
		ReturnPointer(const ReturnPointer<T>& t) : AutoPointer<T>(t, t.retained), retained(t.retained) { }
		template<typename U> ReturnPointer(const ReturnPointer<U>& t) : AutoPointer<T>(CastTo<T>(t), t.retained), retained(t.retained) { }
		template<typename U> ReturnPointer(const Pointer<U>& t, bool retain = true) : AutoPointer<T>(CastTo<T>(t), retain), retained(retain) { }

		~ReturnPointer() { if (!retained && *this) Pointer<T>::Swap(NULL); }

		ReturnPointer<T>& operator=(const Pointer<T>& t) { Swap(t, false); return *this; }
		ReturnPointer<T>& operator=(const AutoPointer<T>& t) { Swap(t, true); return *this; }
		ReturnPointer<T>& operator=(const ReturnPointer<T>& t) { Swap(t, t.retained); return *this; }
		ReturnPointer<T>& operator=(T* t) { Swap(t, false); return *this; }
		template<typename U> ReturnPointer<T>& operator=(const Pointer<U>& t) { Swap(t, false); return *this; }
		template<typename U> ReturnPointer<T>& operator=(const AutoPointer<U>& t) { Swap(t, true); return *this; }

		void Swap(const Pointer<T>& t, bool retain = true) { if (!retained && *this) Pointer<T>::Swap(NULL); AutoPointer<T>::Swap(t, retain); retained = retain; }
		void Release() { if (retained) Release(*this); Pointer<T>::Swap(NULL); }
	};
}
