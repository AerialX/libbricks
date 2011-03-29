#pragma once

#include "bricks.h"
#include "bricks/collections.h"
#include "bricks/collections/stack.h"

#ifndef BRICKS_CONFIG_CPP0X

// TODO: Use non-variadic templates to implement Delegates and remove this limitation.
#error libbricks must be configured with C++0x support to use Delegates and Events.

#else

#include <functional>

#endif

namespace Bricks {
	template<typename F> class Delegate;
	template<typename R, typename... Args> class Delegate<R(Args...)> : public Object
	{
	public:
#ifdef BRICKS_CONFIG_CPP0X
		typedef std::function<R(Args...)> Function;
#else
		typedef R(*Function)(Args...);
#endif
	private:
		Function function;

	public:
		Delegate() { }
		Delegate(Function function) : function(function) { }
		virtual ~Delegate() { }

		virtual R operator()(Args... args) const { return function(args...); }
		R Call(Args... args) const { return self(args...); }
	};

	template<typename C, typename F> class MethodDelegate;
	template<typename C, typename R, typename... Args> class MethodDelegate<C, R(Args...)> : public Delegate<R(Args...)>
	{
	public:
		typedef R (C::*Function)(Args...);

	private:
		AutoPointer<C> pointer;
		Function function;

	public:
		MethodDelegate(C& pointer, Function function) : pointer(pointer), function(function) { }
		virtual ~MethodDelegate() { }

		virtual R operator()(Args... args) const { return (pointer->*function)(args...); }
	};

	template<typename F> class Event;
	template<typename R, typename... Args> class Event<R(Args...)> : public Delegate<void(Args...)>
	{
	public:
		typedef AutoPointer<Delegate<R(Args...)> > EventItem;

	private:
		Pointer<Collections::Collection<EventItem> > list;

	public:
		Event() : list(Alloc(Collections::Stack<EventItem>)) { }
		virtual ~Event() { list->Release(); }

		Event& operator +=(Delegate<R(Args...)>& delegate) { list->AddItem(delegate); return self; }
		Event& operator -=(const Delegate<R(Args...)>& delegate) { list->RemoveItem(delegate); return self; }

		void operator ()(Args... args) const { foreach (const EventItem& item, *list) item->Call(args...); }
	};
}

namespace Bricks { namespace Collections {
	template<typename T> inline void Iterable< T >::Iterate(const Delegate<bool(T&)>& delegate) {
		foreach (T& t, self) {
			if (!delegate(t))
				break;
		}
	}
} }
