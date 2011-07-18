#ifndef BRICKS_ARGLIST_COUNT
#error Use delegate.h
#endif

namespace Bricks {
	template<typename F> class Delegate;
	template<typename F> class Event;

	namespace Internal {
		template<typename F> class BaseDelegate;
		template<typename T, typename F> class Functor;
		template<typename F> class Function;
		template<typename F> class MethodFunctionBase;
		template<typename T, typename F> class MethodFunction;

		template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class BaseDelegate<R(BRICKS_ARGLIST_TYPES)> : public Object
		{
		public:
			virtual R operator ()(BRICKS_ARGLIST_TYPES) = 0;
			R Call(BRICKS_ARGLIST_TYPES_NAMES) const { return const_cast<BaseDelegate<R(BRICKS_ARGLIST_TYPES)>&>(*this)(BRICKS_ARGLIST_ARGS); }
		};

		template<typename T, typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class Functor<T, R(BRICKS_ARGLIST_TYPES)> : public BaseDelegate<R(BRICKS_ARGLIST_TYPES)>
		{
		protected:
			T value;

		public:
			Functor(const T& value) : value(value) { }

			R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { return value(BRICKS_ARGLIST_ARGS); }

			virtual bool operator==(const Object& rhs) const { const Functor<T, R(BRICKS_ARGLIST_TYPES)>* delegate = dynamic_cast<const Functor<T, R(BRICKS_ARGLIST_TYPES)>*>(&rhs); if (delegate) return value == delegate->value; return Object::operator==(rhs); }
			virtual bool operator!=(const Object& rhs) const { return !operator==(rhs); }
		};

		template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class Function<R(BRICKS_ARGLIST_TYPES)> : public BaseDelegate<R(BRICKS_ARGLIST_TYPES)>
		{
		protected:
			typedef R(*FunctionType)(BRICKS_ARGLIST_TYPES);
			FunctionType function;
			friend class Delegate<R(BRICKS_ARGLIST_TYPES)>;

		public:
			Function() : function(NULL) { }
			Function(FunctionType function) : function(function) { }

			R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { return function(BRICKS_ARGLIST_ARGS); }

			virtual bool operator==(const Object& rhs) const { const Function<R(BRICKS_ARGLIST_TYPES)>* delegate = dynamic_cast<const Function<R(BRICKS_ARGLIST_TYPES)>*>(&rhs); if (delegate) return function == delegate->function; return Object::operator==(rhs); }
			virtual bool operator!=(const Object& rhs) const { return !operator==(rhs); }
		};

		template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)> : public BaseDelegate<R(BRICKS_ARGLIST_TYPES)>
		{
		protected:
			void* pointer;

		public:
			MethodFunctionBase(void* pointer) : pointer(pointer) { }

			friend class Event<R(BRICKS_ARGLIST_TYPES)>;

			virtual R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { }
		};

		template<typename T, typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class MethodFunction<T, R(BRICKS_ARGLIST_TYPES)> : public MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)>
		{
		public:
			typedef R (T::*Function)(BRICKS_ARGLIST_TYPES);

		private:
			Function function;

		public:
			MethodFunction(void* pointer, Function function = NULL) : MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)>(pointer), function(function) { }

			R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { return (static_cast<T*>(this->pointer)->*function)(BRICKS_ARGLIST_ARGS); }

			virtual bool operator==(const Object& rhs) const { const MethodFunction<T, R(BRICKS_ARGLIST_TYPES)>* delegate = dynamic_cast<const MethodFunction<T, R(BRICKS_ARGLIST_TYPES)>*>(&rhs); if (delegate) return this->pointer == delegate->pointer && function == delegate->function; return Object::operator==(rhs); }
			virtual bool operator!=(const Object& rhs) const { return !operator==(rhs); }
		};
	}

	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class Delegate<R(BRICKS_ARGLIST_TYPES)> : public Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)>
	{
	protected:
		AutoPointer< Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)> > function;

		friend class Event<R(BRICKS_ARGLIST_TYPES)>;

	public:
		Delegate() { }
		Delegate(Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)>& function) : function(function) { }
		Delegate(typename Internal::Function<R(BRICKS_ARGLIST_TYPES)>::FunctionType function) : function(TempAlloc<Internal::Function<R(BRICKS_ARGLIST_TYPES)> >(function)) { }
		template<typename T> Delegate(const T& function) : function(TempAlloc<Internal::Functor<T, R(BRICKS_ARGLIST_TYPES)> >(function)) { }
		template<typename T> Delegate(T& object, typename Internal::MethodFunction<T, R(BRICKS_ARGLIST_TYPES)>::Function function) : function(TempAlloc<Internal::MethodFunction<T, R(BRICKS_ARGLIST_TYPES)> >(static_cast<void*>(&object), function)) { }

		BRICKS_COPY_CONSTRUCTOR(Delegate<R(BRICKS_ARGLIST_TYPES)>);

		virtual R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { if (function) return function->Call(BRICKS_ARGLIST_ARGS); throw InvalidArgumentException(); }

		bool operator==(const Object& rhs) const { const Delegate<R(BRICKS_ARGLIST_TYPES)>* delegate = dynamic_cast<const Delegate<R(BRICKS_ARGLIST_TYPES)>*>(&rhs); if (delegate) return (!function && !delegate->function) || (function && delegate->function && (*function == *delegate->function)); return Object::operator==(rhs); }
		bool operator !=(const Object& rhs) const { return !operator==(rhs); }
	};

	template<typename T, typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES >
	static Delegate<R(BRICKS_ARGLIST_TYPES)> MethodDelegate(T& object, R (T::*function)(BRICKS_ARGLIST_TYPES)) { return Delegate<R(BRICKS_ARGLIST_TYPES)>(object, function); }

	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class Event<R(BRICKS_ARGLIST_TYPES)> : public Delegate<void(BRICKS_ARGLIST_TYPES)>
	{
	public:
		typedef Delegate< R(BRICKS_ARGLIST_TYPES) > EventItem;
		typedef AutoPointer< EventItem > EventItemStorage;

	private:
		class EventItemComparison : public Object, public Bricks::Collections::ValueComparison< EventItem >
		{
		public:
			Bricks::Collections::ComparisonResult::Enum Compare(const EventItem& v1, const EventItem& v2) {
				typedef const Internal::MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)>* delegate;
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
		Event() : list(TempAlloc<Collections::Array< EventItem, EventItemStorage > >(TempAlloc<EventItemComparison>())) { }

		Event& operator +=(EventItem& delegate) { list->AddItem(delegate); return *this; }
		Event& operator +=(const EventItem& delegate) { list->AddItem(CopyPointer<EventItem>(delegate)); return *this; }
		Event& operator -=(const EventItem& delegate) { list->RemoveItems(delegate); return *this; }
		template<typename T> Event& operator -=(const T& object) { Internal::MethodFunctionBase<R(BRICKS_ARGLIST_TYPES)> method(static_cast<void*>(const_cast<T*>(&object))); list->RemoveItems(Delegate<R(BRICKS_ARGLIST_TYPES)>(static_cast<Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)>&>(method))); return *this; }

		operator bool() const { return list->GetCount(); }

		void operator ()(BRICKS_ARGLIST_TYPES_NAMES) { foreach (EventItem& item, *list) item(BRICKS_ARGLIST_ARGS); }
	};
}
