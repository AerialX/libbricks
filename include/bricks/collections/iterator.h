#pragma once

#include "bricks/object.h"

namespace Bricks {
	template<typename F> class Delegate;
}

namespace Bricks { namespace Collections {
	class InvalidIteratorException : public Exception
	{
	public:
		InvalidIteratorException(const String& message = String::Empty) : Exception(message) { }
	};

	template<typename T> class Collection;

	template<typename T>
	class Iterator : public virtual Object
	{
	public:
		virtual T& GetCurrent() const = 0;
		virtual bool MoveNext() = 0;
		virtual Collection< T >& GetAllObjects() { throw NotImplementedException(); };
	};

	template<typename T>
	class Iterable : public virtual Object
	{
	public:
		virtual Iterator< T >& GetIterator() const = 0;

		void Iterate(const Delegate<bool(T&)>& delegate);
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
		Iterator<T>& value;
		IteratorType(Iterator<T>& value) : IteratorTypeBase(), value(value) { }
	};
	template<typename T>
	inline IteratorType<T> IteratorContainerPack(const Iterable<T>& t) { return IteratorType<T>(t.GetIterator()); }
	template<typename T>
	inline Iterator<T>& IteratorContainerUnpack(const Iterable<T>& dummy, const IteratorTypeBase& t) { return static_cast<const IteratorType<T>&>(t).value; }
} } }
#define foreach_container Bricks::Collections::Internal::IteratorContainerPack
#define foreach_iterator Bricks::Collections::Internal::IteratorContainerUnpack
#define foreach_basetype const Bricks::Collections::Internal::IteratorTypeBase&
#define foreach(val, list) for (foreach_basetype iter = foreach_container(list); foreach_iterator(list, iter).MoveNext() && iter.state;) if (!(iter.state = false)) for (val = foreach_iterator(list, iter).GetCurrent(); !iter.state; iter.state = true)
