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
	template<typename F> class Event;

	namespace Internal {
		template<typename F> class BaseDelegate;
		template<typename T, typename F> class Functor;
		template<typename F> class Function;
		template<typename F> class MethodFunctionBase;
		template<typename T, typename F> class MethodFunction;

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

			virtual bool operator==(const Object& rhs) const { const Functor<T, R(BRICKS_DELEGATE_TYPES)>* delegate = dynamic_cast<const Functor<T, R(BRICKS_DELEGATE_TYPES)>*>(&rhs); if (delegate) return value == delegate->value; return Object::operator==(rhs); }
			virtual bool operator!=(const Object& rhs) const { return !operator==(rhs); }
		};

		template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class Function<R(BRICKS_DELEGATE_TYPES)> : public BaseDelegate<R(BRICKS_DELEGATE_TYPES)>
		{
		protected:
			typedef R(*FunctionType)(BRICKS_DELEGATE_TYPES);
			FunctionType function;
			friend class Delegate<R(BRICKS_DELEGATE_TYPES)>;

		public:
			Function() : function(NULL) { }
			Function(FunctionType function) : function(function) { }

			R operator ()(BRICKS_DELEGATE_TYPES_NAMES) { return function(BRICKS_DELEGATE_ARGS); }

			virtual bool operator==(const Object& rhs) const { const Function<R(BRICKS_DELEGATE_TYPES)>* delegate = dynamic_cast<const Function<R(BRICKS_DELEGATE_TYPES)>*>(&rhs); if (delegate) return function == delegate->function; return Object::operator==(rhs); }
			virtual bool operator!=(const Object& rhs) const { return !operator==(rhs); }
		};

		template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class MethodFunctionBase<R(BRICKS_DELEGATE_TYPES)> : public BaseDelegate<R(BRICKS_DELEGATE_TYPES)>
		{
		protected:
			void* pointer;

		public:
			MethodFunctionBase(void* pointer) : pointer(pointer) { }

			friend class Event<R(BRICKS_DELEGATE_TYPES)>;

			virtual R operator ()(BRICKS_DELEGATE_TYPES_NAMES) { }
		};

		template<typename T, typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class MethodFunction<T, R(BRICKS_DELEGATE_TYPES)> : public MethodFunctionBase<R(BRICKS_DELEGATE_TYPES)>
		{
		public:
			typedef R (T::*Function)(BRICKS_DELEGATE_TYPES);

		private:
			Function function;

		public:
			MethodFunction(void* pointer, Function function = NULL) : MethodFunctionBase<R(BRICKS_DELEGATE_TYPES)>(pointer), function(function) { }

			R operator ()(BRICKS_DELEGATE_TYPES_NAMES) { return (static_cast<T*>(this->pointer)->*function)(BRICKS_DELEGATE_ARGS); }

			virtual bool operator==(const Object& rhs) const { const MethodFunction<T, R(BRICKS_DELEGATE_TYPES)>* delegate = dynamic_cast<const MethodFunction<T, R(BRICKS_DELEGATE_TYPES)>*>(&rhs); if (delegate) return this->pointer == delegate->pointer && function == delegate->function; return Object::operator==(rhs); }
			virtual bool operator!=(const Object& rhs) const { return !operator==(rhs); }
		};
	}

	template<typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES > class Delegate<R(BRICKS_DELEGATE_TYPES)> : public Internal::BaseDelegate<R(BRICKS_DELEGATE_TYPES)>
	{
	protected:
		AutoPointer< Internal::BaseDelegate<R(BRICKS_DELEGATE_TYPES)> > function;

		friend class Event<R(BRICKS_DELEGATE_TYPES)>;

	public:
		Delegate() { }
		Delegate(Internal::BaseDelegate<R(BRICKS_DELEGATE_TYPES)>& function) : function(function) { }
		Delegate(typename Internal::Function<R(BRICKS_DELEGATE_TYPES)>::FunctionType function) : function(alloc Internal::Function<R(BRICKS_DELEGATE_TYPES)>(function), false) { }
		template<typename T> Delegate(const T& function) : function(alloc Internal::Functor<T, R(BRICKS_DELEGATE_TYPES)>(function), false) { }
		template<typename T> Delegate(T& object, typename Internal::MethodFunction<T, R(BRICKS_DELEGATE_TYPES)>::Function function) : function(alloc Internal::MethodFunction<T, R(BRICKS_DELEGATE_TYPES)>(static_cast<void*>(&object), function), false) { }

		BRICKS_COPY_CONSTRUCTOR(Delegate<R(BRICKS_DELEGATE_TYPES)>);

		virtual R operator ()(BRICKS_DELEGATE_TYPES_NAMES) { if (function) return function->Call(BRICKS_DELEGATE_ARGS); throw InvalidArgumentException(); }

		bool operator==(const Object& rhs) const { const Delegate<R(BRICKS_DELEGATE_TYPES)>* delegate = dynamic_cast<const Delegate<R(BRICKS_DELEGATE_TYPES)>*>(&rhs); if (delegate) return (!function && !delegate->function) || (function && delegate->function && (*function == *delegate->function)); return Object::operator==(rhs); }
		bool operator !=(const Object& rhs) const { return !operator==(rhs); }
	};

	template<typename T, typename R BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES >
	static Delegate<R(BRICKS_DELEGATE_TYPES)> MethodDelegate(T& object, R (T::*function)(BRICKS_DELEGATE_TYPES)) { return Delegate<R(BRICKS_DELEGATE_TYPES)>(object, function); }

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
				typedef const Internal::MethodFunctionBase<R(BRICKS_DELEGATE_TYPES)>* delegate;
				delegate d1 = dynamic_cast<delegate>(v1.function.GetValue());
				if (d1) {
					delegate d2 = dynamic_cast<delegate>(v2.function.GetValue());
					if (d2)
						return d1->pointer == d2->pointer ? Bricks::Collections::ComparisonResult::Equal : Bricks::Collections::ComparisonResult::Less;
				}
				return v1 == v2 ? Bricks::Collections::ComparisonResult::Equal : Bricks::Collections::ComparisonResult::Less;
			}
		};
		AutoPointer<Collections::Collection< EventItem > > list;

	public:
		Event() : list(alloc Collections::Array< EventItem, EventItemStorage >(AutoPointer<EventItemComparison>(alloc EventItemComparison(), false)), false) { }

		Event& operator +=(EventItem& delegate) { list->AddItem(delegate); return self; }
		Event& operator +=(const EventItem& delegate) { list->AddItem(CopyPointer<EventItem>(delegate)); return self; }
		Event& operator -=(const EventItem& delegate) { list->RemoveItems(delegate); return self; }
		template<typename T> Event& operator -=(const T& object) { Internal::MethodFunctionBase<R(BRICKS_DELEGATE_TYPES)> method(static_cast<void*>(const_cast<T*>(&object))); list->RemoveItems(Delegate<R(BRICKS_DELEGATE_TYPES)>(static_cast<Internal::BaseDelegate<R(BRICKS_DELEGATE_TYPES)>&>(method))); return self; }

		operator bool() const { return list->GetCount(); }

		void operator ()(BRICKS_DELEGATE_TYPES_NAMES) { foreach (EventItem& item, *list) item(BRICKS_DELEGATE_ARGS); }
	};

	template<typename T BRICKS_DELEGATE_COMMA BRICKS_DELEGATE_TYPENAMES >
	static AutoPointer<T> Autorelease(BRICKS_DELEGATE_TYPES_NAMES) { return AutoPointer<T>(alloc T(BRICKS_DELEGATE_ARGS), false); }
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
