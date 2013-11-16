#pragma once

#include "bricks/core/sfinae.h"
#include "bricks/core/autopointer.h"
#include "bricks/core/copypointer.h"

#include "bricks/collections/list.h"

namespace Bricks { namespace Collections {
	template<typename T> class ListGuardBase;
	template<typename T> class Array;
	template<typename T> class AutoArray;

	namespace Internal {
		template<typename T, typename E = void> struct ListGuardArray;
		template<typename T> struct ListGuardArray<T, typename SFINAE::DisableIf<SFINAE::IsCompatibleType<AutoArray<typename SFINAE::MakeValueType<typename T::IteratorType>::Type>, T>::Value>::Type> { typedef Array<typename T::IteratorType> Type; };
		template<typename T> struct ListGuardArray<T, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<AutoArray<typename SFINAE::MakeValueType<typename T::IteratorType>::Type>, T>::Value>::Type> { typedef AutoArray<typename SFINAE::MakeValueType<typename T::IteratorType>::Type> Type; };

		template<typename T, typename E = void> class ListGuardIteratorHolder;

		template<typename T>
		class ListGuardIteratorHolder<T, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<IterableFastBase, T>::Value >::Type >
		{
			protected:
				typedef typename T::IteratorType I;
				typename T::IteratorFastType iterator;

			public:
				ListGuardIteratorHolder(ListGuardBase<T>* list) : iterator(list->list->GetIteratorFast()) { }

				I& GetCurrent() const { return iterator.GetCurrent(); }
				bool MoveNext() { return iterator.MoveNext(); }
		};

		template<typename T>
		class ListGuardIteratorHolder<T, typename SFINAE::DisableIf<SFINAE::IsCompatibleType<IterableFastBase, T>::Value >::Type >
		{
			protected:
				typedef typename T::IteratorType I;
				AutoPointer<Iterator<I> > iterator;

			public:
				ListGuardIteratorHolder(ListGuardBase<T>* list) : iterator(list->list->GetIterator()) { }

				I& GetCurrent() const { return iterator->GetCurrent(); }
				bool MoveNext() const { return iterator->MoveNext(); }
		};

		template<typename T>
		class ListGuardIterator : public Iterator<typename T::IteratorType>
		{
			protected:
				typedef typename T::IteratorType I;
				typedef typename ListGuardArray<T>::Type CacheType;

				ListGuardBase<T>* list;
				ListGuardIteratorHolder<T> iterator;

				AutoPointer<CacheType> listCache;
				AutoPointer<typename Array<I>::IteratorFastType> iteratorCache;
				bool status;

				ListGuardIterator(const ListGuardBase<T>* list) :
					list(const_cast<ListGuardBase<T>*>(list)),
					iterator(this->list),
					status(false)
				{
					this->list->IteratorRegister(this);
				}

				void ListChanged()
				{
					if (iteratorCache || !list)
						return;
					listCache = autonew CacheType();
					if (status || !iterator.MoveNext()) {
						do
							listCache->AddItem(iterator.GetCurrent());
						while (iterator.MoveNext());
					}
					iteratorCache = autonew typename Array<I>::IteratorFastType(listCache->GetIteratorFast());
					if (status)
						iteratorCache->MoveNext();
				}

				friend class ListGuardBase<T>;

			public:
				I& GetCurrent() const
				{
					return iteratorCache ? iteratorCache->GetCurrent() : iterator.GetCurrent();
				}

				bool MoveNext()
				{
					if (iteratorCache ? iteratorCache->MoveNext() : iterator.MoveNext())
						return status = true;
					if (list)
						list->IteratorUnregister(this);
					list = NULL;
					return false;
				}

				ListGuardIterator& operator =(const ListGuardIterator& copy)
				{
					if (list)
						list->IteratorUnregister(this);
					list = copy.list;
					iterator = copy.iterator;
					listCache = copy.listCache;
					iteratorCache = copy.iteratorCache;
					status = copy.status;
					if (list)
						list->IteratorRegister(this);
					return *this;
				}

				ListGuardIterator(const ListGuardIterator& copy) :
					list(copy.list),
					iterator(copy.iterator),
					listCache(copy.listCache),
					iteratorCache(copy.iteratorCache),
					status(copy.status)
				{
					list->IteratorRegister(this);
				}

				~ListGuardIterator()
				{
					if (list)
						list->IteratorUnregister(this);
				}
		};
	}

	template<typename T>
	class ListGuardBase : public List<typename T::IteratorType>, public IterableFast< Internal::ListGuardIterator<T> >
	{
	protected:
		typedef Internal::ListGuardIterator<T> GuardIterator;
		typedef typename T::IteratorType I;

		AutoPointer<T> list;
		Array<GuardIterator*> iterators;

		void GuardIterators()
		{
			BRICKS_FOR_EACH (GuardIterator* iterator, iterators)
				iterator->ListChanged();
		}

		void DestroyIterators()
		{
			BRICKS_FOR_EACH (GuardIterator* iterator, iterators) {
				iterator->ListChanged();
				iterator->list = NULL;
			}
		}

		void IteratorRegister(GuardIterator* iterator) { iterators.AddItem(iterator); }
		void IteratorUnregister(GuardIterator* iterator) { iterators.RemoveItem(iterator); }

		friend class Internal::ListGuardIterator<T>;
		friend class Internal::ListGuardIteratorHolder<T>;

	public:
		ListGuardBase(T* list) : list(list) { }
		ListGuardBase(const ListGuardBase& list) : list(list.list) { }
		~ListGuardBase() { DestroyIterators(); }

		virtual ListGuardBase& operator =(const ListGuardBase& copy) { GuardIterators(); list = copy.list; return *this; }

		// Collection
		virtual long GetCount() const { return list->GetCount(); }

		virtual bool ContainsItem(const I& value) const { return list->ContainsItem(value); }

		virtual void AddItem(const I& value) { GuardIterators(); list->AddItem(value); }
		virtual void AddItems(Iterable<I>* values) { GuardIterators(); list->AddItems(values); }
		virtual bool RemoveItem(const I& value) { GuardIterators(); return list->RemoveItem(value); }
		virtual void RemoveItems(const I& value) { GuardIterators(); list->RemoveItems(value); }
		virtual void Clear() { GuardIterators(); list->Clear(); }

		// List
		virtual void SetItem(long index, const I& value) { GuardIterators(); list->SetItem(index, value); }

		virtual const I& GetItem(long index) const { return list->GetItem(index); }
		virtual I& GetItem(long index) { return list->GetItem(index); }
		virtual long IndexOfItem(const I& value) const { return list->IndexOfItem(value); }

		virtual void InsertItem(long index, const I& value) { GuardIterators(); list->InsertItem(index, value); }
		virtual void RemoveItemAt(long index) { GuardIterators(); list->RemoveItemAt(index); }

		// Iterator
		virtual ReturnPointer<Iterator<I> > GetIterator() const { return autonew GuardIterator(this); }
		virtual GuardIterator GetIteratorFast() const { return GuardIterator(this); }

		bool IsGuarding() const { return iterators.GetCount(); }
	};

	template<typename T, typename E = void> class ListGuard;
	template<typename T> class ListGuard<T, typename SFINAE::EnableIf<SFINAE::HasConstructor<T>::Value>::Type> : public ListGuardBase<T>
	{
	public:
		ListGuard() : ListGuardBase<T>(autonew T()) { }
		ListGuard(T* list) : ListGuardBase<T>(list) { }
	};

	template<typename T> class ListGuard<T, typename SFINAE::DisableIf<SFINAE::HasConstructor<T>::Value>::Type> : public ListGuardBase<T>
	{
	public:
		ListGuard(T* list) : ListGuardBase<T>(list) { }
	};

	template<typename T> class ArrayGuard : public ListGuard<Array<T> > { };
	template<typename T> class AutoArrayGuard : public ListGuard<AutoArray<T> > { };
} }
