#pragma once

#include "bricks/core/autopointer.h"
#include "bricks/collections/dictionary.h"

namespace Bricks { namespace Threading {
	namespace Internal {
		typedef Collections::Dictionary<const void*, AutoPointer<Object> > ThreadLocalStorageDictionary;
		ThreadLocalStorageDictionary& GetStorageDictionary();
	}

	template<typename T>
	class ThreadLocalStorage
	{
	public:
		ThreadLocalStorage() { }
		ThreadLocalStorage(const T& value) { Internal::GetStorageDictionary().Add(this, value); }
		~ThreadLocalStorage() { Internal::GetStorageDictionary().RemoveKey(this); }

		bool HasValue() const { return Internal::GetStorageDictionary().ContainsKey(this); }
		T* GetValue() const { return dynamic_cast<T*>(Internal::GetStorageDictionary()[this].GetValue()); }
		void SetValue(T* value) { Internal::GetStorageDictionary().Add(this, value); }
	};
} }
