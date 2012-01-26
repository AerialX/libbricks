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

		AutoArray& operator =(const AutoArray<T>& array) { ReleaseAll(); Array<T*>::operator=(array); RetainAll(); return *this; }

		void AddItem(T*const& value) { Array<T*>::AddItem(value); Retain(value); }
		bool RemoveItem(T*const& value) { long index = Array<T*>::IndexOfItem(value); if (index >= 0) { T* item = Array<T*>::GetItem(index); Array<T*>::RemoveItemAt(index); Release(item); return true; } return false; }

		void Clear() { ReleaseAll(); Array<T*>::Clear(); }

		void SetItem(long index, T*const& value) { Release(Array<T*>::GetItem(index)); Array<T*>::SetItem(index, value); Retain(value); }

		void InsertItem(long index, T*const& value) { Array<T*>::InsertItem(index, value); Retain(value); }
		void RemoveItemAt(long index) { Release(Array<T*>::GetItem(index)); Array<T*>::RemoveItemAt(index); }
	};
} }
