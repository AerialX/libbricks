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

#ifdef BRICKS_CONFIG_CPP0X
#include <functional>
#endif

namespace Bricks {
	template<typename F> class Delegate;
	template<typename F, typename T> class FunctorDelegate;
	template<typename C, typename F> class MethodDelegate;
	template<typename F> class Event;

	namespace Internal {
		template<typename F> class BaseDelegate;
		template<typename T, typename F> class Functor;

		template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class BaseDelegate<R(BRICKS_DELEGATE_TYPES)> : public virtual Object
		{
		public:
			virtual R operator ()(BRICKS_DELEGATE_TYPES) = 0;
			R Call(BRICKS_DELEGATE_TYPES_NAMES) const { return const_cast<BaseDelegate<R(BRICKS_DELEGATE_TYPES)>&>(self)(BRICKS_DELEGATE_ARGS); }
		};

		template<typename T, typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class Functor<T, R(BRICKS_DELEGATE_TYPES)> : public BaseDelegate<R(BRICKS_DELEGATE_TYPES)>
		{
		protected:
			T value;

		public:
			Functor(const T& value) : value(value) { }

			R operator ()(BRICKS_DELEGATE_TYPES_NAMES) { return value(BRICKS_DELEGATE_ARGS); }
		};
	}

	template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class Delegate<R(BRICKS_DELEGATE_TYPES)> : public Internal::BaseDelegate<R(BRICKS_DELEGATE_TYPES)>
	{
	protected:
		typedef R(*Function)(BRICKS_DELEGATE_TYPES);
		AutoPointer< Internal::BaseDelegate<R(BRICKS_DELEGATE_TYPES)> > functor;
		Function function;

	public:
		template<typename T> Delegate(const T& functor) : functor(alloc Internal::Functor<T, R(BRICKS_DELEGATE_TYPES)>(functor), false), function(NULL) { }
		Delegate(Function function) : function(function) { }
		Delegate() : function(NULL) { }

		virtual R operator ()(BRICKS_DELEGATE_TYPES_NAMES) { if (functor) return functor->Call(BRICKS_DELEGATE_ARGS); if (function) return function(BRICKS_DELEGATE_ARGS); throw InvalidArgumentException(); }
	};

	namespace Internal {
		template<typename F> class MethodDelegateBase;

		template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class MethodDelegateBase<R(BRICKS_DELEGATE_TYPES)> : public Delegate<R(BRICKS_DELEGATE_TYPES)>
		{
		protected:
			AutoPointer<Object> pointer;

		public:
			MethodDelegateBase(const Pointer<Object>& pointer) : pointer(pointer) { }

			friend class Event<R(BRICKS_DELEGATE_TYPES)>;

			virtual R operator ()(BRICKS_DELEGATE_TYPES_NAMES) { }
		};
	}

	template<typename C, typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class MethodDelegate<C, R(BRICKS_DELEGATE_TYPES)> : public Internal::MethodDelegateBase<R(BRICKS_DELEGATE_TYPES)>
	{
	public:
		typedef R (C::*Function)(BRICKS_DELEGATE_TYPES);

	private:
		Function function;

	public:
		MethodDelegate(const Pointer< C >& pointer, Function function = NULL) : Internal::MethodDelegateBase<R(BRICKS_DELEGATE_TYPES)>(pointer), function(function) { }

		R operator ()(BRICKS_DELEGATE_TYPES_NAMES) { return (static_cast<C*>(this->pointer.GetValue())->*function)(BRICKS_DELEGATE_ARGS); }

		bool operator==(const Object& rhs) const { const MethodDelegate<C, R(BRICKS_DELEGATE_TYPES)>* delegate = dynamic_cast<MethodDelegate<C, R(BRICKS_DELEGATE_TYPES)>*>(&rhs); if (delegate) return this->pointer == delegate.pointer && function == delegate.function; return Object::operator==(rhs); }
		bool operator!=(const Object& rhs) const { return !operator==(rhs); }
	};

	template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class Event<R(BRICKS_DELEGATE_TYPES)> : public Delegate<void(BRICKS_DELEGATE_TYPES)>
	{
	public:
		typedef Delegate< R(BRICKS_DELEGATE_TYPES) > EventItem;
		typedef AutoPointer< EventItem > EventItemStorage;

	private:
		class EventItemComparison : public Bricks::Collections::ValueComparison< EventItem >
		{
		public:
			Bricks::Collections::ComparisonResult::Enum Compare(const EventItem& v1, const EventItem& v2) {
				typedef const Internal::MethodDelegateBase<R(BRICKS_DELEGATE_TYPES)>* delegate;
				delegate d1 = dynamic_cast<delegate>(&*v1);
				if (d1) {
					delegate d2 = dynamic_cast<delegate>(&*v2);
					if (d2)
						return d1->pointer == d2->pointer ? Bricks::Collections::ComparisonResult::Equal : Bricks::Collections::ComparisonResult::Less;
				}
				return v1 == v2 ? Bricks::Collections::ComparisonResult::Equal : Bricks::Collections::ComparisonResult::Less;
			}
		};
		AutoPointer<Collections::Collection< EventItem > > list;

	public:
		Event() : list(alloc Collections::Stack< EventItem, EventItemStorage >(), false) { }

		Event& operator +=(const Pointer< Delegate<R(BRICKS_DELEGATE_TYPES)> >& delegate) { list->AddItem(delegate); return self; }
		Event& operator -=(const EventItem& delegate) { list->RemoveItems(delegate); return self; }
		Event& operator -=(const Pointer<Object>& object) { list->RemoveItems(Internal::MethodDelegateBase<R(BRICKS_DELEGATE_TYPES)>(object)); return self; }

		operator bool() const { return list->GetCount(); }

		void operator ()(BRICKS_DELEGATE_TYPES_NAMES) { foreach (EventItem& item, *list) item(BRICKS_DELEGATE_ARGS); }
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
