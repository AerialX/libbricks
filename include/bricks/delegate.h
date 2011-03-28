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
		virtual R operator()(Args...) const = 0;
		R Call(Args... args) const { return self(args...); }
	};

	template<typename F> class FunctionDelegate;
	template<typename R, typename... Args> class FunctionDelegate<R(Args...)> : public Delegate<R(Args...)>
	{
	public:
		typedef R (*Function)(Args...);

	private:
		Function function;

	public:
		FunctionDelegate(Function function) : function(function) { }
		FunctionDelegate(const FunctionDelegate& function) : function(function.function) { }

		R operator ()(Args... args) const { return function(args...); }
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
		MethodDelegate(const MethodDelegate& function) : pointer(function.pointer), function(function.function) { }

		R operator()(Args... args) const { return (pointer->*function)(args...); }
	};
	
	template<typename C, typename F> class FunctorDelegate;
	template<typename C, typename R, typename... Args> class FunctorDelegate<C, R(Args...)> : public Delegate<R(Args...)>
	{
	public:
		typedef C Function;

	private:
		Function function;

	public:
		FunctorDelegate(const Function& function) : function(function) { }
		FunctorDelegate(const FunctorDelegate& function) : function(function.function) { }

		R operator ()(Args... args) const { return function(args...); }
	};

#ifdef BRICKS_CONFIG_CPP0X
	template<typename F> class StandardDelegate;
	template<typename R, typename... Args> class StandardDelegate<R(Args...)> : public FunctorDelegate<std::function<R(Args...)>, R(Args...)>
	{
	public:
		StandardDelegate(const std::function<R(Args...)>& function) : FunctorDelegate<std::function<R(Args...)>, R(Args...)>(function) { }
		StandardDelegate(const StandardDelegate& function) : FunctorDelegate<std::function<R(Args...)>, R(Args...)>(function) { }
	};
#endif

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
	template<typename T> inline void Iterable< T >::Iterate(Delegate<bool(T&)>& delegate) {
		foreach (T& t, self) {
			if (!delegate(t))
				break;
		}
	}
} }
