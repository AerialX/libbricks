#ifndef BRICKS_ARGLIST_COUNT
#error Use jmethoddelegate.h
#endif

namespace Bricks { namespace Java {
	template<typename F> class JMethodDelegate;
	template<typename F> class JBoundMethodDelegate;
	template<typename F> class JStaticMethodDelegate;
	template<typename F> class JBoundStaticMethodDelegate;

	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class JMethodDelegate<R(BRICKS_ARGLIST_TYPES)> : public Bricks::Internal::BaseDelegate<typename Internal::TypeConversion<R>::Type(JReference* BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPES)>
	{
	protected:
		AutoPointer<JMethod> method;

	public:
		typedef R(JSignature)(BRICKS_ARGLIST_TYPES);

		JMethodDelegate()
		{

		}

		JMethodDelegate(JMethod* method) :
			method(method)
		{

		}

		JMethod* GetMethod() const { return method; }

		virtual typename Internal::TypeConversion<R>::Type operator ()(JReference* object BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPES_NAMES)
		{
			jvalue argArray[BRICKS_ARGLIST_COUNT];
			BRICKS_JAVA_JMETHODDELEGATE(BRICKS_ARGLIST_COUNT, argArray, arg);
			return method->CallArray<R>(object, argArray);
		}
	};

	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class JBoundMethodDelegate<R(BRICKS_ARGLIST_TYPES)> : public Bricks::Internal::BaseDelegate<typename Internal::TypeConversion<R>::Type(BRICKS_ARGLIST_TYPES)>
	{
	protected:
		AutoPointer<JObject> object;
		AutoPointer<JMethod> method;
		String name;

	public:
		typedef R(JSignature)(BRICKS_ARGLIST_TYPES);

		JBoundMethodDelegate()
		{

		}

		JBoundMethodDelegate(JObject* object, JMethod* method) :
			object(object), method(method)
		{

		}

		JBoundMethodDelegate(JObject* object, const String& name) :
			object(object), name(name)
		{

		}

		JObject* GetObject() const { return object; }
		void SetObject(JObject* value) { object = value; }

		JMethod* GetMethod() const { return method; }
		void SetMethod(JMethod* value) { method = value; }

		const String& GetName() const { return name; }
		void SetName(const String& value) { name = value; }

		virtual typename Internal::TypeConversion<R>::Type operator ()(BRICKS_ARGLIST_TYPES_NAMES)
		{
			if (!method)
				method = object->GetClass()->GetMethod(name, Internal::JSignature<JSignature>::Signature());
			jvalue argArray[BRICKS_ARGLIST_COUNT];
			BRICKS_JAVA_JMETHODDELEGATE(BRICKS_ARGLIST_COUNT, argArray, arg);
			return method->CallArray<R>(object, argArray);
		}
	};

	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class JStaticMethodDelegate<R(BRICKS_ARGLIST_TYPES)> : public Bricks::Internal::BaseDelegate<typename Internal::TypeConversion<R>::Type(BRICKS_ARGLIST_TYPES)>
	{
	protected:
		AutoPointer<JStaticMethod> method;

	public:
		typedef R(JSignature)(BRICKS_ARGLIST_TYPES);

		JStaticMethodDelegate()
		{

		}

		JStaticMethodDelegate(JStaticMethod* method) :
			method(method)
		{

		}

		JStaticMethod* GetMethod() const { return method; }

		virtual typename Internal::TypeConversion<R>::Type operator ()(BRICKS_ARGLIST_TYPES_NAMES)
		{
			jvalue argArray[BRICKS_ARGLIST_COUNT];
			BRICKS_JAVA_JMETHODDELEGATE(BRICKS_ARGLIST_COUNT, argArray, arg);
			return method->CallArray<R>(argArray);
		}
	};

	template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > class JBoundStaticMethodDelegate<R(BRICKS_ARGLIST_TYPES)> : public Bricks::Internal::BaseDelegate<typename Internal::TypeConversion<R>::Type(BRICKS_ARGLIST_TYPES)>
	{
	protected:
		AutoPointer<JClass> clazz;
		AutoPointer<JStaticMethod> method;
		String name;

	public:
		typedef R(JSignature)(BRICKS_ARGLIST_TYPES);

		JBoundStaticMethodDelegate()
		{

		}

		JBoundStaticMethodDelegate(JStaticMethod* method) :
			method(method)
		{

		}

		JClass* GetClass() const { return clazz; }
		void SetClass(JClass* value) { clazz = value; }

		JStaticMethod* GetMethod() const { return method; }
		void SetMethod(JStaticMethod* value) { method = value; }

		const String& GetName() const { return name; }
		void SetName(const String& value) { name = value; }

		virtual typename Internal::TypeConversion<R>::Type operator ()(BRICKS_ARGLIST_TYPES_NAMES)
		{
			if (!method)
				method = clazz->GetStaticMethod(name, Internal::JSignature<JSignature>::Signature());
			jvalue argArray[BRICKS_ARGLIST_COUNT];
			BRICKS_JAVA_JMETHODDELEGATE(BRICKS_ARGLIST_COUNT, argArray, arg);
			return method->CallArray<R>(argArray);
		}
	};

	namespace Internal {
		template<typename F> struct JConstructorMethodDelegateTransform;
		template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > struct JConstructorMethodDelegateTransform<R(BRICKS_ARGLIST_TYPES)> { typedef void(Signature)(BRICKS_ARGLIST_TYPES); typedef R Type; };

		template<typename R BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES > struct JSignature<R(BRICKS_ARGLIST_TYPES)> {
			static String Signature()
			{
				String signature("(");
				BRICKS_JAVA_JSIGNATURE(BRICKS_ARGLIST_COUNT, signature, T);
				return signature + ")" + JSignature<R>::Signature();
			}
		};
	}

	template<typename T BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_TYPENAMES >
	inline typename Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Type JClass::Construct(BRICKS_ARGLIST_TYPES_NAMES)
	{
		ReturnPointer<JObject> instance = Allocate();
		GetConstructor<typename Internal::JConstructorMethodDelegateTransform<T>::Signature>()(instance BRICKS_ARGLIST_COMMA BRICKS_ARGLIST_ARGS);
		return Internal::TypeConversion<typename Internal::JConstructorMethodDelegateTransform<T>::Type>::Convert(instance);
	}
} }
