#pragma once

#include "bricks.h"
#include "bricks/collections.h"
#include "bricks/collections/stack.h"

namespace Bricks {
	template<typename F> class Delegate;
	template<typename R, typename... Args> class Delegate<R(Args...)> : public Object
	{
	public:
		virtual R operator()(Args...) const = 0;
		R Call(Args... args) { return self(args...); }
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

		void operator ()(Args... args) const { foreach (const EventItem& item, *list) (*item)(args...); }
	};
}
