#pragma once

#include "bricks.h"
#include "bricks/collections.h"
#include "bricks/collections/stack.h"

#define BRICKS_HEADER_DELEGATE

#ifndef BRICKS_CONFIG_CPP0X

#define BRICKS_DELEGATE_COUNT 0
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 1
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 2
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 3
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 4
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 5
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 6
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 7
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 8
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 9
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT

#else
#include <functional>

namespace Bricks {
	template<typename F> class Delegate;
	template<typename R, typename... Args> class Delegate<R(Args...)> : public virtual Object
	{
	public:
		Delegate() { }
		virtual ~Delegate() { }

		virtual R operator ()(Args... args) = 0;
		R Call(Args... args) { return self(args...); }
	};
	
	template<typename F> class FunctionDelegate;
	template<typename R, typename... Args> class FunctionDelegate<R(Args...)> : public Delegate<R(Args...)>
	{
	public:
		typedef std::function<R(Args...)> Function;

	private:
		Function function;

	public:
		FunctionDelegate() { }
		FunctionDelegate(Function function) : function(function) { }

		R operator ()(Args... args) { if (!function) throw InvalidArgumentException(); return function(args...); }
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

		R operator ()(Args... args) { return (pointer->*function)(args...); }
	};

	template<typename F> class Event;
	template<typename R, typename... Args> class Event<R(Args...)> : public Delegate<void(Args...)>
	{
	public:
		typedef AutoPointer<Delegate<R(Args...)> > EventItem;

	private:
		AutoPointer<Collections::Collection<EventItem> > list;

	public:
		Event() : list(alloc Collections::Stack<EventItem>(), false) { }

		Event& operator +=(Delegate<R(Args...)>* delegate) { list->AddItem(delegate); return self; }
		Event& operator -=(const Delegate<R(Args...)>* delegate) { list->RemoveItem(delegate); return self; }

		void operator ()(Args... args) { foreach (const EventItem& item, list) item->Call(args...); }
	};
}
#endif

namespace Bricks { namespace Collections {
	template<typename T> inline void Iterable< T >::Iterate(const Delegate<bool(T&)>& delegate) {
		foreach (T& t, self) {
			if (!delegate(t))
				break;
		}
	}
} }
