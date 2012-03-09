#include "bricks/threading/threadlocalstorage.h"

#include <pthread.h>

#define BRICKS_PTHREAD_KEY_REF CastToRaw<pthread_key_t>(storageHandle)
#define BRICKS_PTHREAD_KEY (*CastToRaw<pthread_key_t>(storageHandle))

namespace Bricks { namespace Threading {
	namespace Internal {
		class ThreadLocalStorageData
		{
		protected:
			ThreadLocalStorage* storage;
			void* value;

		public:
			ThreadLocalStorageData(ThreadLocalStorage* storage, void* value = NULL) :
				storage(storage), value(value)
			{

			}

			ThreadLocalStorage* GetStorage() const { return storage; }
			void* GetValue() const { return value; }
			void SetValue(void* newValue) { value = newValue; }
		};

		ThreadLocalStorage::ThreadLocalStorage()
		{
			storageHandle = CastToRaw(new pthread_key_t());
			pthread_key_create(BRICKS_PTHREAD_KEY_REF, &ThreadLocalStorage::StaticDestructor);
		}

		ThreadLocalStorage::~ThreadLocalStorage()
		{
			pthread_key_delete(BRICKS_PTHREAD_KEY);
			delete BRICKS_PTHREAD_KEY_REF;
		}

		void ThreadLocalStorage::StaticDestructor(void* data)
		{
			ThreadLocalStorageData* storage = CastToRaw<ThreadLocalStorageData>(data);
			if (storage)
				storage->GetStorage()->Destroy(storage);
		}

		void ThreadLocalStorage::Destroy(ThreadLocalStorageData* data)
		{
			Destroy(data->GetValue());
			delete data;
		}

		void ThreadLocalStorage::Destroy(void* data)
		{

		}

		ThreadLocalStorageData* ThreadLocalStorage::GetInternalData(bool create) const
		{
			void* data = pthread_getspecific(BRICKS_PTHREAD_KEY);
			if (!data && create) {
				data = new ThreadLocalStorageData(tempnew *this);
				pthread_setspecific(BRICKS_PTHREAD_KEY, data);
			}
			return CastToRaw<ThreadLocalStorageData>(data);
		}

		void* ThreadLocalStorage::GetData() const
		{
			ThreadLocalStorageData* data = GetInternalData(false);
			if (data)
				return data->GetValue();
			return NULL;
		}

		void ThreadLocalStorage::SetData(void* value)
		{
			GetInternalData(true)->SetValue(value);
		}

		bool ThreadLocalStorage::HasData() const
		{
			return GetInternalData(false);
		}
	}
} }
