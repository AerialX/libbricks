#ifndef BRICKS_HEADER_DELEGATE
#error Use delegate.h
#endif

#if BRICKS_DELEGATE_COUNT == 0
#define BRICKS_DELEGATE_COMMA
#else
#define BRICKS_DELEGATE_COMMA ,
#endif

#define BRICKS_DELEGATE_ENUM_0(m, ...)
#define BRICKS_DELEGATE_ENUM_1(m, ...) m(1, __VA_ARGS__)
#define BRICKS_DELEGATE_ENUM_2(m, ...) BRICKS_DELEGATE_ENUM_1(m, __VA_ARGS__), m(2, __VA_ARGS__)
#define BRICKS_DELEGATE_ENUM_3(m, ...) BRICKS_DELEGATE_ENUM_2(m, __VA_ARGS__), m(3, __VA_ARGS__)
#define BRICKS_DELEGATE_ENUM_4(m, ...) BRICKS_DELEGATE_ENUM_3(m, __VA_ARGS__), m(4, __VA_ARGS__)
#define BRICKS_DELEGATE_ENUM_5(m, ...) BRICKS_DELEGATE_ENUM_4(m, __VA_ARGS__), m(5, __VA_ARGS__)
#define BRICKS_DELEGATE_ENUM_6(m, ...) BRICKS_DELEGATE_ENUM_5(m, __VA_ARGS__), m(6, __VA_ARGS__)
#define BRICKS_DELEGATE_ENUM_7(m, ...) BRICKS_DELEGATE_ENUM_6(m, __VA_ARGS__), m(7, __VA_ARGS__)
#define BRICKS_DELEGATE_ENUM_8(m, ...) BRICKS_DELEGATE_ENUM_7(m, __VA_ARGS__), m(8, __VA_ARGS__)
#define BRICKS_DELEGATE_ENUM_9(m, ...) BRICKS_DELEGATE_ENUM_8(m, __VA_ARGS__), m(9, __VA_ARGS__)

#define BRICKS_DELEGATE_ENUM(count, value) BRICKS_DELEGATE_ENUM_(count, value)
#define BRICKS_DELEGATE_ENUM2(count, value1, value2) BRICKS_DELEGATE_ENUM2_(count, value1, value2)
#define BRICKS_DELEGATE_ENUM_(count, value) BRICKS_DELEGATE_ENUM_ ## count(BRICKS_DELEGATE_ENUM_METHOD_1, value)
#define BRICKS_DELEGATE_ENUM2_(count, value1, value2) BRICKS_DELEGATE_ENUM_ ## count(BRICKS_DELEGATE_ENUM_METHOD_2, value1, value2)
#define BRICKS_DELEGATE_ENUM_METHOD_1(n, v) v ## n
#define BRICKS_DELEGATE_ENUM_METHOD_2(n, v1, v2) v1 ## n v2 ## n

#define BRICKS_DELEGATE_TYPENAMES BRICKS_DELEGATE_ENUM(BRICKS_DELEGATE_COUNT, typename T)
#define BRICKS_DELEGATE_TYPES BRICKS_DELEGATE_ENUM(BRICKS_DELEGATE_COUNT, T)
#define BRICKS_DELEGATE_TYPES_NAMES BRICKS_DELEGATE_ENUM2(BRICKS_DELEGATE_COUNT, T, arg)
#define BRICKS_DELEGATE_ARGS BRICKS_DELEGATE_ENUM(BRICKS_DELEGATE_COUNT, arg)

namespace Bricks {
	template<typename F> class Delegate;
	template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class Delegate<R(BRICKS_DELEGATE_TYPES)> : public Object
	{
	public:
		Delegate() { }
		virtual ~Delegate() { }

		virtual R operator ()(BRICKS_DELEGATE_TYPES) const = 0;
		R Call(BRICKS_DELEGATE_TYPES_NAMES) const { return self(BRICKS_DELEGATE_ARGS); }
	};
	
	template<typename F> class FunctionDelegate;
	template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES> class FunctionDelegate<R(BRICKS_DELEGATE_TYPES)> : public Delegate<R(BRICKS_DELEGATE_TYPES)>
	{
	public:
#ifdef BRICKS_CONFIG_CPP0X
		typedef std::function<R(BRICKS_DELEGATE_TYPES)> Function;
#else
		typedef R(*Function)(BRICKS_DELEGATE_TYPES);
#endif
	private:
		Function function;

	public:
		FunctionDelegate() { }
		FunctionDelegate(Function function) : function(function) { }

		R operator ()(BRICKS_DELEGATE_TYPES_NAMES) const { if (!function) throw InvalidArgumentException(); return function(BRICKS_DELEGATE_ARGS); }
	};

	template<typename C, typename F> class MethodDelegate;
	template<typename C, typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class MethodDelegate<C, R(BRICKS_DELEGATE_TYPES)> : public Delegate<R(BRICKS_DELEGATE_TYPES)>
	{
	public:
		typedef R (C::*Function)(BRICKS_DELEGATE_TYPES);

	private:
		AutoPointer<C> pointer;
		Function function;

	public:
		MethodDelegate(C& pointer, Function function) : pointer(pointer), function(function) { }

		R operator ()(BRICKS_DELEGATE_TYPES_NAMES) const { return (pointer->*function)(BRICKS_DELEGATE_ARGS); }
	};

	template<typename F> class Event;
	template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class Event<R(BRICKS_DELEGATE_TYPES)> : public Delegate<void(BRICKS_DELEGATE_TYPES)>
	{
	public:
		typedef AutoPointer<Delegate<R(BRICKS_DELEGATE_TYPES)> > EventItem;

	private:
		AutoPointer<Collections::Collection<EventItem> > list;

	public:
		Event() : list(alloc Collections::Stack<EventItem>(), false) { }

		Event& operator +=(Delegate<R(BRICKS_DELEGATE_TYPES)>* delegate) { list->AddItem(delegate); return self; }
		Event& operator -=(const Delegate<R(BRICKS_DELEGATE_TYPES)>* delegate) { list->RemoveItem(delegate); return self; }

		void operator ()(BRICKS_DELEGATE_TYPES_NAMES) const { foreach (const EventItem& item, list) item->Call(BRICKS_DELEGATE_ARGS); }
	};
}

#undef BRICKS_DELEGATE_COMMA
#undef BRICKS_DELEGATE_ENUM
#undef BRICKS_DELEGATE_ENUM2
#undef BRICKS_DELEGATE_ENUM_
#undef BRICKS_DELEGATE_ENUM2_
#undef BRICKS_DELEGATE_ENUM_METHOD_1
#undef BRICKS_DELEGATE_ENUM_METHOD_2
#undef BRICKS_DELEGATE_TYPENAMES
#undef BRICKS_DELEGATE_TYPES
#undef BRICKS_DELEGATE_TYPES_NAMES
#undef BRICKS_DELEGATE_ARGS
#undef BRICKS_DELEGATE_ENUM_0
#undef BRICKS_DELEGATE_ENUM_1
#undef BRICKS_DELEGATE_ENUM_2
#undef BRICKS_DELEGATE_ENUM_3
#undef BRICKS_DELEGATE_ENUM_4
#undef BRICKS_DELEGATE_ENUM_5
#undef BRICKS_DELEGATE_ENUM_6
#undef BRICKS_DELEGATE_ENUM_7
#undef BRICKS_DELEGATE_ENUM_8
#undef BRICKS_DELEGATE_ENUM_9
