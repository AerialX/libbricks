#pragma once

#include "bricks/java/jobject.h"
#include "bricks/java/jmethoddelegate.h"
#include "bricks/collections/array.h"

namespace Bricks { namespace Java { namespace Internal {
		template<typename T> struct JArrayTypeConversion;
#define BRICKS_JAVA_JARRAYTYPECONVERSION(T, JT) \
		template<> struct JArrayTypeConversion<T> { typedef JT##Array Type; }
		BRICKS_JAVA_TYPES_CONVERSION(BRICKS_JAVA_JARRAYTYPECONVERSION);
#undef BRICKS_JAVA_JARRAYTYPECONVERSION
		template<typename T> struct JArrayValueTypeConversion;
#define BRICKS_JAVA_JARRAYVALUETYPECONVERSION(T, JT) \
		template<> struct JArrayValueTypeConversion<T> { typedef JT Type; }
		BRICKS_JAVA_TYPES_CONVERSION(BRICKS_JAVA_JARRAYVALUETYPECONVERSION);
#undef BRICKS_JAVA_JARRAYTYPECONVERSION
		template<typename T> struct JArrayType;
#define BRICKS_JAVA_JARRAYTYPE(T, name) \
		template<> struct JArrayType<T> \
		{ \
			typedef JArrayTypeConversion<T>::Type JType; \
			typedef JArrayValueTypeConversion<T>::Type Type; \
			static jsize GetLength(JVM* vm, JType array) { return vm->GetEnv()->GetArrayLength(array); } \
			static JType CreateArray(JVM* vm, jsize length) { return vm->GetEnv()->New##name##Array(length); } \
			static Type* GetItems(JVM* vm, JType array) { return vm->GetEnv()->Get##name##ArrayElements(array, NULL); } \
			static void ReleaseItems(JVM* vm, JType array, Type* items) { vm->GetEnv()->Release##name##ArrayElements(array, items, 0); } \
			static void GetItems(JVM* vm, JType array, Type* buffer, jsize offset, jsize length) { vm->GetEnv()->Get##name##ArrayRegion(array, offset, length, buffer); } \
			static void SetItems(JVM* vm, JType array, const Type* buffer, jsize offset, jsize length) { vm->GetEnv()->Set##name##ArrayRegion(array, offset, length, buffer); } \
		}
		BRICKS_JAVA_TYPES(BRICKS_JAVA_JARRAYTYPE);
#undef BRICKS_JAVA_JARRAYTYPE
} } }

namespace Bricks { namespace Java { namespace Lang {
	template<typename T, typename E = void> class Array : public JObject
	{
	public:
		typedef typename Internal::JArrayType<T>::JType JType;
		typedef typename Internal::JArrayType<T>::Type Type;

	protected:
		void CreateArray(const T* data, int length)
		{
			reference = Internal::JArrayType<T>::CreateArray(vm, length);
			if (data)
				SetItems(data, 0, length);
			clazz = autonew JClass(vm, reference);
		}

		void CreateArray(const Collections::List<T>* array)
		{
			CreateArray(NULL, array->GetCount());
			Type* items = Internal::JArrayType<T>::GetItems(vm, GetJArray());
			Type* itemsIter = items;
			BRICKS_FOR_EACH (const T item, array)
				*(itemsIter++) = item;
			Internal::JArrayType<T>::ReleaseItems(vm, GetJArray(), items);
		}

	public:
		Array(JVM* vm, const T* data, int length) :
			JObject(vm, NULL)
		{
			CreateArray(data, length);
		}

		Array(const T* data, int length) :
			JObject(JVM::GetCurrentVM(), NULL)
		{
			CreateArray(data, length);
		}

		Array(JVM* vm, int length) :
			JObject(vm, NULL)
		{
			CreateArray(NULL, length);
		}

		Array(int length) :
			JObject(JVM::GetCurrentVM(), NULL)
		{
			CreateArray(NULL, length);
		}

		Array(JVM* vm, const Collections::List<T>* array) :
			JObject(vm, NULL)
		{
			CreateArray(array);
		}

		Array(const Collections::List<T>* array) :
			JObject(JVM::GetCurrentVM(), NULL)
		{
			CreateArray(array);
		}

		Array(JClass* clazz, jobject object, bool global = false) :
			JObject(clazz, object, global)
		{

		}

		Array(JVM* vm, jobject object, bool global = false) :
			JObject(vm, object, global)
		{

		}

		Array(JReference* reference) :
			JObject(reference)
		{

		}

		JType GetJArray() const { return (JType)GetObject(); }

		ReturnPointer<Collections::Array<T> > GetArray() const;

		int GetCount()
		{
			return Internal::JArrayType<T>::GetLength(vm, GetJArray());
		}

		T GetItem(int index)
		{
			Type ret;
			Internal::JArrayType<T>::GetItems(vm, GetJArray(), &ret, index, 1);
			return ret;
		}

		void SetItem(int index, T value)
		{
			Internal::JArrayType<T>::SetItems(vm, GetJArray() &value, index, 1);
		}

		void GetItems(T* buffer, int offset, int length)
		{
			Internal::JArrayType<T>::GetItems(vm, GetJArray(), buffer, offset, length);
		}

		void SetItems(const T* buffer, int offset, int length)
		{
			Internal::JArrayType<T>::SetItems(vm, GetJArray(), buffer, offset, length);
		}
	};

	template<typename T> class Array<T, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<JObject, T>::Value>::Type> : public JObject
	{
	public:
		typedef jobjectArray JType;

	protected:
		template<typename U>
		void CreateArray(int length, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<Internal::JQualifiedObjectBase, U>::Value, JClass>::Type* dummy = NULL)
		{
			reference = vm->GetEnv()->NewObjectArray(length, T::Class::GetClass(vm), NULL);
			clazz = autonew JClass(vm, reference);
		}

		void CreateArray(int length, JClass* clazz)
		{
			reference = vm->GetEnv()->NewObjectArray(length, clazz->GetClass(), NULL);
			clazz = autonew JClass(vm, reference);
		}

		template<typename U>
		void CreateArray(const Collections::List<U>* array, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<JReference, U>::Value>::Type* dummy = NULL)
		{
			int index = 0;
			BRICKS_FOR_EACH (const U& item, array)
				vm->GetEnv()->SetObjectArrayElement(GetJArray(), index++, item->GetReference());
		}

		template<typename U>
		void CreateArray(const Collections::List<U*>* array, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<JReference, U>::Value>::Type* dummy = NULL)
		{
			int index = 0;
			BRICKS_FOR_EACH (const U* item, array)
				vm->GetEnv()->SetObjectArrayElement(GetJArray(), index++, item->GetReference());
		}

	public:
		Array(JVM* vm, int length) : JObject(vm, NULL) { CreateArray<T>(length); }
		Array(int length) : JObject(JVM::GetCurrentVM(), NULL) { CreateArray<T>(length); }

		template<typename U>
		Array(JVM* vm, const Collections::List<U>* array) : JObject(vm, NULL) { CreateArray<T>(array->GetCount()); CreateArray(array); }
		template<typename U>
		Array(const Collections::List<U>* array) : JObject(JVM::GetCurrentVM(), NULL) { CreateArray<T>(array->GetCount()); CreateArray(array); }

		Array(JVM* vm, JClass* clazz, int length) : JObject(vm, NULL) { CreateArray(length, clazz); }
		Array(JClass* clazz, int length) : JObject(JVM::GetCurrentVM(), NULL) { CreateArray(length, clazz); }

		template<typename U>
		Array(JVM* vm, JClass* clazz, const Collections::List<U>* array) : JObject(vm, NULL) { CreateArray(array->GetCount(), clazz); CreateArray(array); }
		template<typename U>
		Array(JClass* clazz, const Collections::List<U>* array) : JObject(JVM::GetCurrentVM(), NULL) { CreateArray(array->GetCount(), clazz); CreateArray(array); }

		Array(JClass* clazz, jobject object, bool global = false) : JObject(clazz, object, global) { }
		Array(JVM* vm, jobject object, bool global = false) : JObject(vm, object, global) { }
		Array(JReference* reference) : JObject(reference) { }

		JType GetJArray() const { return (JType)GetObject(); }

		ReturnPointer<Collections::Array<T> > GetArray() const;

		int GetCount()
		{
			return Internal::JArrayType<T>::GetLength(vm, GetJArray());
		}

		T GetItem(int index)
		{
			return Internal::TypeConversion<T>::Convert(vm->GetEnv()->GetObjectArrayElement(GetJArray(), index));
		}

		void SetItem(int index, T value)
		{
			vm->GetEnv()->SetObjectArrayElement(GetJArray(), index, Internal::TypeConversion<T>::trevnoC(value));
		}
	};
} } }

namespace Bricks { namespace Java { namespace Internal {
		template<typename T> struct JSignature<Lang::Array<T> > { static String Signature() { return "[" + JSignature<T>::Signature(); } };
		template<typename T> struct JSignature<Collections::Array<T> > { static String Signature() { return JSignature<Lang::Array<T> >::Signature(); } };
		template<typename T> struct TypeConversion<Collections::Array<T> >
		{
			typedef ReturnPointer<Collections::Array<T> > Type;

			static Type Convert(JReference* value)
			{
				return Lang::Array<T>(value).GetArray();
			}

			static jobject trevnoC(const Collections::Array<T>* value)
			{
				return Lang::Array<T>(value).GetReference();
			}
		};
} } }
