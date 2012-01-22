#ifndef BRICKS_ARGLIST_COUNT
#error Use delegate.h
#endif

#ifdef BRICKS_FEATURE_OBJC_BLOCKS
#include <Block.h>
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
		template<typename F> class ObjCBlock;

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

			template<typename U>
			static typename SFINAE::EnableIf<SFINAE::HasEqualityOperator<U, U>::Value, bool>::Type CompareValues(const U& lhs, const U& rhs) { return lhs == rhs; }
			template<typename U>
			static typename SFINAE::DisableIf<SFINAE::HasEqualityOperator<U, U>::Value, bool>::Type CompareValues(const U& lhs, const U& rhs) { return &lhs == &rhs; }

		public:
			Functor(const T& value) : value(value) { }

			R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { return value(BRICKS_ARGLIST_ARGS); }

			virtual bool operator==(const Object& rhs) const { const Functor<T, R(BRICKS_ARGLIST_TYPES)>* delegate = dynamic_cast<const Functor<T, R(BRICKS_ARGLIST_TYPES)>*>(&rhs); if (delegate) return CompareValues(value, delegate->value); return Object::operator==(rhs); }
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

			virtual R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { BRICKS_FEATURE_RELEASE_THROW_FATAL(InvalidArgumentException()); }
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

#ifdef BRICKS_FEATURE_OBJC_BLOCKS
		template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class ObjCBlock<R(BRICKS_ARGLIST_TYPES)> : public BaseDelegate<R(BRICKS_ARGLIST_TYPES)>
		{
		protected:
			typedef R(^FunctionType)(BRICKS_ARGLIST_TYPES);
			FunctionType function;

			friend class Delegate<R(BRICKS_ARGLIST_TYPES)>;

		public:
			ObjCBlock(FunctionType function) : function(Block_copy(function)) { }
			ObjCBlock(const ObjCBlock<R(BRICKS_ARGLIST_TYPES)>& block) : function(Block_copy(block.function)) { }
			~ObjCBlock() { Block_release(function); }

			ObjCBlock<R(BRICKS_ARGLIST_TYPES)>& operator =(const ObjCBlock<R(BRICKS_ARGLIST_TYPES)>& block) { Block_release(function); function = Block_copy(function); return *this; }

			R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { return function(BRICKS_ARGLIST_ARGS); }
		};
#endif
	}

	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class Delegate<R(BRICKS_ARGLIST_TYPES)> : public Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)>
	{
	protected:
		AutoPointer< Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)> > function;

		friend class Event<R(BRICKS_ARGLIST_TYPES)>;

	public:
		Delegate() { }
		Delegate(Internal::BaseDelegate<R(BRICKS_ARGLIST_TYPES)>& function) : function(function) { }
		Delegate(typename Internal::Function<R(BRICKS_ARGLIST_TYPES)>::FunctionType function) : function(autonew Internal::Function<R(BRICKS_ARGLIST_TYPES)>(function)) { }
		template<typename T> Delegate(const T& function) : function(autonew Internal::Functor<T, R(BRICKS_ARGLIST_TYPES)>(function)) { }
		template<typename T> Delegate(T* object, typename Internal::MethodFunction<T, R(BRICKS_ARGLIST_TYPES)>::Function function) : function(autonew Internal::MethodFunction<T, R(BRICKS_ARGLIST_TYPES)>(static_cast<void*>(object), function)) { }
#ifdef BRICKS_FEATURE_OBJC_BLOCKS
		Delegate(typename Internal::ObjCBlock<R(BRICKS_ARGLIST_TYPES)>::FunctionType function) : function(autonew Internal::ObjCBlock<R(BRICKS_ARGLIST_TYPES)>(function)) { }
#endif

		BRICKS_COPY_CONSTRUCTOR(Delegate<R(BRICKS_ARGLIST_TYPES)>);

		virtual R operator ()(BRICKS_ARGLIST_TYPES_NAMES) { if (function) return function->Call(BRICKS_ARGLIST_ARGS); BRICKS_FEATURE_RELEASE_THROW_FATAL(InvalidArgumentException()); }

		operator bool() const { return function; }
		bool operator==(const Object& rhs) const { const Delegate<R(BRICKS_ARGLIST_TYPES)>* delegate = dynamic_cast<const Delegate<R(BRICKS_ARGLIST_TYPES)>*>(&rhs); if (delegate) return (!function && !delegate->function) || (function && delegate->function && (*function == *delegate->function)); return Object::operator==(rhs); }
		bool operator!=(const Object& rhs) const { return !operator==(rhs); }
	};

	template<typename T, typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES >
	static inline Delegate<R(BRICKS_ARGLIST_TYPES)> MethodDelegate(T* object, R (T::*function)(BRICKS_ARGLIST_TYPES)) { return Delegate<R(BRICKS_ARGLIST_TYPES)>(object, function); }
	template<typename T, typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES >
	static inline Delegate<R(BRICKS_ARGLIST_TYPES)> MethodDelegate(T& object, R (T::*function)(BRICKS_ARGLIST_TYPES)) { return Delegate<R(BRICKS_ARGLIST_TYPES)>(&object, function); }
	template<typename T, typename U, typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES >
	static inline Delegate<R(BRICKS_ARGLIST_TYPES)> MethodDelegate(const Pointer<T>& object, R (U::*function)(BRICKS_ARGLIST_TYPES)) { return Delegate<R(BRICKS_ARGLIST_TYPES)>(object.GetValue(), function); }
	template<typename T, typename U, typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES >
	static inline Delegate<R(BRICKS_ARGLIST_TYPES)> MethodDelegate(const Pointer<T>& object, R (U::*function)(BRICKS_ARGLIST_TYPES) const) { return Delegate<R(BRICKS_ARGLIST_TYPES)>(object.GetValue(), (R(U::*)(BRICKS_ARGLIST_TYPES))function); }
}
