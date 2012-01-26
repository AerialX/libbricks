#pragma once

#include "bricks/core/autopointer.h"

namespace Bricks {
	template<typename T> class CopyPointer : public AutoPointer<T>
	{
#define BRICKS_COPY_POINTER(t) ((t) ? autonew T(t) : NULL)
	public:
		CopyPointer() { }
		CopyPointer(const CopyPointer<T>& t) : AutoPointer<T>(BRICKS_COPY_POINTER(t)) { }
		CopyPointer(const Pointer<T>& t) : AutoPointer<T>(BRICKS_COPY_POINTER(t)) { }
		CopyPointer(const T* t) : AutoPointer<T>(BRICKS_COPY_POINTER(t)) { }

		CopyPointer<T>& operator=(const Pointer<T>& t) { Swap(t); return *this; }
		CopyPointer<T>& operator=(const CopyPointer<T>& t) { Swap(t); return *this; }
		CopyPointer<T>& operator=(const T* t) { AutoPointer<T>::Swap(BRICKS_COPY_POINTER(t)); return *this; }

		void Swap(const Pointer<T>& t) { AutoPointer<T>::Swap(BRICKS_COPY_POINTER(t)); }
#undef BRICKS_COPY_POINTER
	};

	class NoCopy { private: NoCopy(const NoCopy&); NoCopy& operator =(const NoCopy&); protected: NoCopy() { } };
}
