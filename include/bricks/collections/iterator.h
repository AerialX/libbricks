#pragma once

#include "bricks/object.h"

namespace Bricks { namespace Collections {
	class InvalidIteratorException : public Exception
	{
	public:
		InvalidIteratorException(const String& message = String::Empty) : Exception(message) { }
	};

	template<typename T> class Collection;

	template<typename T>
	class Iterator
	{
	public:
		virtual T& GetCurrent() const = 0;
		virtual bool MoveNext() = 0;
		virtual ReturnPointer< Collection< T > > GetAllObjects() { throw NotImplementedException(); };
	};

	template<typename T>
	class Iterable
	{
	public:
		typedef T IteratorType;

		virtual ReturnPointer< Iterator< IteratorType > > GetIterator() const = 0;

		void Iterate(const Delegate<bool(IteratorType&)>& delegate) const;
		void Iterate(const Delegate<void(IteratorType&)>& delegate) const;
	};
} }

namespace Bricks { namespace Collections { namespace Internal {
	struct IteratorTypeBase
	{
		mutable bool state;
		operator bool() const { return state; }
		IteratorTypeBase() : state(true) { }
	};
	template<typename T>
	struct IteratorType : public IteratorTypeBase
	{
		AutoPointer< Iterator<T> > value;
		IteratorType(const Pointer< Iterator<T> >& value) : value(value) { }
	};
	template<typename T>
	static inline IteratorType<T> IteratorContainerPack(const Iterable<T>& t) { return IteratorType<T>(t.GetIterator()); }
	template<typename T>
	static inline IteratorType<T> IteratorContainerPack(const Iterable<T>* t) { return IteratorType<T>(t->GetIterator()); }
	template<typename T>
	static inline IteratorType<typename T::IteratorType> IteratorContainerPack(const Pointer<T>& t) { return IteratorContainerPack(*t); }

	template<typename T>
	static inline Iterator<T>& IteratorContainerUnpack(const Iterable<T>& dummy, const IteratorTypeBase& t) { return *static_cast<const IteratorType<T>&>(t).value; }
	template<typename T>
	static inline Iterator<T>& IteratorContainerUnpack(const Iterable<T>* dummy, const IteratorTypeBase& t) { return *static_cast<const IteratorType<T>&>(t).value; }
	template<typename T>
	static inline Iterator<typename T::IteratorType>& IteratorContainerUnpack(const Pointer<T>& dummy, const IteratorTypeBase& t) { return IteratorContainerUnpack(*dummy, t); }
} } }
#define foreach_container Bricks::Collections::Internal::IteratorContainerPack
#define foreach_iterator Bricks::Collections::Internal::IteratorContainerUnpack
#define foreach_basetype const Bricks::Collections::Internal::IteratorTypeBase&
#define foreach(val, list) for (foreach_basetype iter = foreach_container(list); foreach_iterator(list, iter).MoveNext() && iter.state;) if (!(iter.state = false)) for (val = foreach_iterator(list, iter).GetCurrent(); !iter.state; iter.state = true)

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
