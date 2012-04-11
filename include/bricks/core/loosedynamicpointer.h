#pragma once

#include "bricks/core/pointer.h"

namespace Bricks {
	template<typename T = Object> class LooseDynamicPointer : public Pointer<T>
	{
	public:
		LooseDynamicPointer() { }
		LooseDynamicPointer(const LooseDynamicPointer<T>& t) : Pointer<T>(t) { }
		LooseDynamicPointer(const Pointer<T>& t) : Pointer<T>(t) { }
		LooseDynamicPointer(T* t) : Pointer<T>(t) { }
		template<typename U> LooseDynamicPointer(const Pointer<U>& t) : Pointer<T>(CastToDynamic<T>(t)) { }
		template<typename U> LooseDynamicPointer(U* t) : Pointer<T>(CastToDynamic<T>(t)) { }

		LooseDynamicPointer<T>& operator=(const Pointer<T>& t) { Swap(t); return *this; }
		LooseDynamicPointer<T>& operator=(const LooseDynamicPointer<T>& t) { Swap(t); return *this; }
		LooseDynamicPointer<T>& operator=(T* t) { Swap(t); return *this; }
		template<typename U> LooseDynamicPointer<T>& operator=(const Pointer<U>& t) { Swap(CastToDynamic<T>(t)); return *this; }
		template<typename U> LooseDynamicPointer<T>& operator=(U* t) { Swap(CastToDynamic<T>(t)); return *this; }
	};
}
