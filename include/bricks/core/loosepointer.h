#pragma once

#include "bricks/core/pointer.h"

namespace Bricks {
	template<typename T = Object> class LoosePointer : public Pointer<T>
	{
	public:
		LoosePointer() { }
		LoosePointer(const LoosePointer<T>& t) : Pointer<T>(t) { }
		LoosePointer(const Pointer<T>& t) : Pointer<T>(t) { }
		LoosePointer(T* t) : Pointer<T>(t) { }
		template<typename U> LoosePointer(const Pointer<U>& t) : Pointer<T>(CastTo<T>(t)) { }
		template<typename U> LoosePointer(U* t) : Pointer<T>(CastTo<T>(t)) { }

		LoosePointer<T>& operator=(const Pointer<T>& t) { Swap(t); return *this; }
		LoosePointer<T>& operator=(const LoosePointer<T>& t) { Swap(t); return *this; }
		LoosePointer<T>& operator=(T* t) { Swap(t); return *this; }
		template<typename U> LoosePointer<T>& operator=(const Pointer<U>& t) { Swap(CastTo<T>(t)); return *this; }
		template<typename U> LoosePointer<T>& operator=(U* t) { Swap(CastTo<T>(t)); return *this; }
	};
}
