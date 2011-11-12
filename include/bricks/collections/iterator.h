#pragma once

#include "bricks/object.h"

namespace Bricks { namespace Collections {
	namespace Internal {
		class IterableBase { };
		class IterableFastBase { };
	}
	class InvalidIteratorException : public Exception
	{
	public:
		InvalidIteratorException(const String& message = String::Empty) : Exception(message) { }
	};

	template<typename T> class Collection;

	template<typename T>
	class Iterator : public Object
	{
	public:
		typedef T IteratorType;

		virtual T& GetCurrent() const = 0;
		virtual bool MoveNext() = 0;
		virtual ReturnPointer< Collection< T > > GetAllObjects() { BRICKS_FEATURE_THROW(NotImplementedException()); };
	};

	template<typename T>
	class Iterable : public Internal::IterableBase
	{
	public:
		typedef T IteratorType;

		virtual ReturnPointer< Iterator< IteratorType > > GetIterator() const = 0;

		void Iterate(const Delegate<bool(IteratorType&)>& delegate) const;
		void Iterate(const Delegate<void(IteratorType&)>& delegate) const;
	};

	template<typename T>
	class IterableFast : public Internal::IterableFastBase
	{
	public:
		typedef T IteratorFastType;

		virtual T GetIteratorFast() const = 0;
	};
} }

namespace Bricks { namespace Collections { namespace Internal {
	template<typename T>
	struct IteratorType
	{
		template<typename U> IteratorType(const U& list, typename SFINAE::DisableIf<!SFINAE::IsCompatibleType<IterableBase, U>::Value>::Type* dummy = NULL) : state(true), iter(list.GetIterator()) { }
		template<typename U> IteratorType(const U* list) : state(true), iter(list->GetIterator()) { }
		template<typename U> IteratorType(const Pointer<U>& list) : state(true), iter(list->GetIterator()) { }
		bool state;
		AutoPointer<Iterator<T> > iter;
		inline bool MoveNext() const { return iter->MoveNext(); }
		inline T& GetCurrent() const { return iter->GetCurrent(); }
	};
	template<typename T>
	struct IteratorFastType
	{
		template<typename U> IteratorFastType(const U& list, typename SFINAE::DisableIf<!SFINAE::IsCompatibleType<IterableFastBase, U>::Value>::Type* dummy = NULL) : state(true), iter(list.GetIteratorFast()) { }
		template<typename U> IteratorFastType(const U* list) : state(true), iter(list->GetIteratorFast()) { }
		template<typename U> IteratorFastType(const Pointer<U>& list) : state(true), iter(list->GetIteratorFast()) { }
		bool state;
		T iter;
		inline bool MoveNext() const { return const_cast<T&>(iter).MoveNext(); }
		inline typename T::IteratorType& GetCurrent() const { return iter.GetCurrent(); }
	};

	template<typename T> static typename SFINAE::EnableIf<SFINAE::IsCompatibleType<IterableBase, T>::Value && !SFINAE::IsCompatibleType<IterableFastBase, T>::Value, IteratorType<typename T::IteratorType> >::Type IteratorContainerType(const T& t);
	template<typename T> static typename SFINAE::EnableIf<SFINAE::IsCompatibleType<IterableBase, T>::Value && !SFINAE::IsCompatibleType<IterableFastBase, T>::Value, IteratorType<typename T::IteratorType> >::Type IteratorContainerType(const T* t);
	template<typename T> static typename SFINAE::EnableIf<SFINAE::IsCompatibleType<IterableBase, T>::Value && !SFINAE::IsCompatibleType<IterableFastBase, T>::Value, IteratorType<typename T::IteratorType> >::Type IteratorContainerType(const Pointer<T>& t);

	template<typename T> static typename SFINAE::EnableIf<SFINAE::IsCompatibleType<IterableFastBase, T>::Value, IteratorFastType<typename T::IteratorFastType> >::Type IteratorContainerType(const T& t);
	template<typename T> static typename SFINAE::EnableIf<SFINAE::IsCompatibleType<IterableFastBase, T>::Value, IteratorFastType<typename T::IteratorFastType> >::Type IteratorContainerType(const T* t);
	template<typename T> static typename SFINAE::EnableIf<SFINAE::IsCompatibleType<IterableFastBase, T>::Value, IteratorFastType<typename T::IteratorFastType> >::Type IteratorContainerType(const Pointer<T>& t);
} } }
#define foreach(val, list) for (typeof(Bricks::Collections::Internal::IteratorContainerType(list)) iter(list); iter.MoveNext() && iter.state;) if (!(iter.state = false)) for (val = iter.GetCurrent(); !iter.state; iter.state = true)

namespace Bricks { namespace Collections {
	template<typename T> inline void Iterable<T>::Iterate(const Delegate<bool(Iterable<T>::IteratorType&)>& delegate) const {
		foreach (IteratorType& t, *this) {
			if (!delegate.Call(t))
				break;
		}
	}

	template<typename T> inline void Iterable<T>::Iterate(const Delegate<void(Iterable<T>::IteratorType&)>& delegate) const {
		foreach (IteratorType& t, *this)
			delegate.Call(t);
	}
} }
