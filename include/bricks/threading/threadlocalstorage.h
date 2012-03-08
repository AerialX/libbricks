#pragma once

#include "bricks/core/autopointer.h"
#include "bricks/core/copypointer.h"
#include "bricks/core/exception.h"

namespace Bricks { namespace Threading {
	namespace Internal {
		class ThreadLocalStorageData;

		class ThreadLocalStorage : public Object, NoCopy
		{
		protected:
			void* storageHandle;

			ThreadLocalStorageData* GetInternalData(bool create) const;
			static void StaticDestructor(void* data);

			void Destroy(ThreadLocalStorageData* data);
			virtual void Destroy(void* data);

			ThreadLocalStorageData* GetIntenalData() const;

		public:
			ThreadLocalStorage();
			~ThreadLocalStorage();

			void* GetData() const;
			void SetData(void* value);
			bool HasData() const;
		};
	}

	template<typename T>
	class ThreadLocalStorage : public Internal::ThreadLocalStorage
	{
	protected:
		void Destroy(void* data) {
			delete CastToRaw<T>(data);
		}

	public:
		ThreadLocalStorage() { }

		bool HasValue() const { return HasData(); }

		T& GetValue() const {
			if (!HasValue())
				BRICKS_FEATURE_THROW(Exception());
			return *CastToRaw<T>(GetData());
		}

		void SetValue(const T& value) {
			if (!HasValue())
				SetData(new T(value));
			else
				*CastToRaw<T>(GetData()) = value;
		}
	};

	template<typename T>
	class AutoThreadLocalStorage : public Internal::ThreadLocalStorage
	{
	protected:
		void Destroy(void* data) {
			AutoPointer<>::Release(CastToRaw<T>(data));
		}

	public:
		AutoThreadLocalStorage() { }

		bool HasValue() const { return HasData(); }
		T* GetValue() const {
			if (!HasValue())
				return NULL;
			return CastToRaw<T>(GetData());
		}

		void SetValue(T* value) {
			if (HasValue())
				Destroy(GetData());
			SetData(CastToRaw(value));
			AutoPointer<>::Retain(value);
		}
	};
} }
