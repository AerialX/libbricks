#pragma once

#include "bricks/collections/array.h"

namespace Bricks { namespace Collections {
	template<typename T>
	class AutoArray : public Array<T*>
	{
	protected:
		static void Retain(T* value) { AutoPointer<>::Retain(value); }
		static void Release(T* value) { AutoPointer<>::Release(value); }
		void RetainAll() { foreach (T*const& item, this) Retain(item); }
		void ReleaseAll() { foreach (T*const& item, this) Release(item); }

	public:
		AutoArray(const AutoArray<T>& array, ValueComparison<T*>* comparison = NULL) : Array<T*>(array, comparison) { RetainAll(); }
		AutoArray(ValueComparison<T*>* comparison = autonew OperatorValueComparison<T*>()) : Array<T*>(comparison) { }
		AutoArray(Iterable<T*>* iterable, ValueComparison<T*>* comparison = autonew OperatorValueComparison<T*>()) : Array<T*>(iterable, comparison) { RetainAll(); }

		~AutoArray() { ReleaseAll(); }

		AutoArray& operator =(const AutoArray<T>& array) { Array<T*> toRelease(*this); Array<T*>::operator=(array); RetainAll(); foreach (T*const& item, toRelease) Release(item); return *this; }

		void AddItem(T*const& value) { Array<T*>::AddItem(value); Retain(value); }
		bool RemoveItem(T*const& value) { long index = Array<T*>::IndexOfItem(value); if (index >= 0) { T* item = Array<T*>::GetItem(index); Array<T*>::RemoveItemAt(index); Release(item); return true; } return false; }
		void RemoveItems(T*const& value) { Retain(value); while (RemoveItem(value)) ; Release(value); }

		void Clear() { ReleaseAll(); Array<T*>::Clear(); }

		void SetItem(long index, T*const& value) { T* item = Array<T*>::GetItem(index);Array<T*>::SetItem(index, value); Release(item); Retain(value); }

		void InsertItem(long index, T*const& value) { Array<T*>::InsertItem(index, value); Retain(value); }
		void RemoveItemAt(long index) { T* item = Array<T*>::GetItem(index); Array<T*>::RemoveItemAt(index); Release(item); }
	};
} }
