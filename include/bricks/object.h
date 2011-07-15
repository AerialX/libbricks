#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

#include <stdlib.h>

#ifdef BRICKS_CONFIG_RTTI
#include <typeinfo>
#endif

#define BRICKS_COPY_CONSTRUCTOR(T) public: virtual T& Copy() const { return Alloc< T >(*this); }
#define BRICKS_COPY_HIDE(T) private: T(const T&); T& operator=(const T&);

namespace Bricks {
	class Class;
	class String;
	class Object;

	namespace Internal {
		struct ObjectAlloc { };
		extern ObjectAlloc Auto;
		struct ObjectGlobalAlloc { };
		extern ObjectGlobalAlloc Global;

		class ReferenceCounter
		{
		private:
			u32 referenceCount;
			ReferenceCounter() : referenceCount(1) { }
			ReferenceCounter(const ReferenceCounter& count) : referenceCount(1) { }
			ReferenceCounter& operator =(const ReferenceCounter& count) { return *this; }
			u32 operator ++(int) { return referenceCount++; }
			u32 operator ++() { return ++referenceCount; }
			u32 operator --() { return --referenceCount; }
			u32 operator --(int) { return referenceCount--; }
			operator u32() const { return referenceCount; }
			friend class Bricks::Object;
		};
	}

	class Object
	{
	private:
		Internal::ReferenceCounter referenceCount;

		static bool InternalAutorelease();
#ifdef BRICKS_CONFIG_LOGGING_MEMLEAK
		static void ReportObject(Object* object, bool status);
		static void ReportObject(bool status);
#define BRICKS_FEATURE_LOGGING_MEMLEAK() ReportObject(this, true)
#define BRICKS_FEATURE_LOGGING_MEMLEAK_DESTROY() do { if (referenceCount == 1) ReportObject(this, false); } while (false)
#define BRICKS_FEATURE_LOGGING_MEMLEAK_CREATE() ReportObject(true)
#else
#define BRICKS_FEATURE_LOGGING_MEMLEAK()
#define BRICKS_FEATURE_LOGGING_MEMLEAK_DESTROY()
#define BRICKS_FEATURE_LOGGING_MEMLEAK_CREATE()
#endif

		static void* mallocthrowable(size_t size);

	public:
		Object() { BRICKS_FEATURE_LOGGING_MEMLEAK(); }
		virtual ~Object() { }

#ifdef BRICKS_CONFIG_RTTI
		Class& GetClass() const;
		template<class T> bool IsSubclassOf() const { return dynamic_cast< T >(this); }
		template<class T> bool IsClassOf() const { return typeid(this) == typeid(T); }
#endif

		Object& Autorelease();
		Object& Retain() { referenceCount++; BRICKS_FEATURE_LOG_HEAVY("+%p [%d]", this, GetReferenceCount()); return *this; }
		void Release() { BRICKS_FEATURE_LOG_HEAVY("-%p [%d]", this, GetReferenceCount() - 1); if (!--referenceCount) delete this; BRICKS_FEATURE_LOGGING_MEMLEAK_DESTROY(); }
		int GetReferenceCount() const { return referenceCount; }
		template<typename T> T& Autorelease() { return static_cast<T&>(Autorelease()); }
		template<typename T> T& Retain() { return static_cast<T&>(Retain()); }

		virtual bool operator==(const Object& rhs) const { return this == &rhs; }
		virtual bool operator!=(const Object& rhs) const { return this != &rhs; }

		void* operator new(size_t size, const Internal::ObjectGlobalAlloc& dummy) { return mallocthrowable(size); }
		void* operator new(size_t size) { BRICKS_FEATURE_LOGGING_MEMLEAK_CREATE(); return mallocthrowable(size); }
		void operator delete(void* data) { free(data); }

#undef BRICKS_FEATURE_LOGGING_MEMLEAK
#undef BRICKS_FEATURE_LOGGING_MEMLEAK_DESTROY
#undef BRICKS_FEATURE_LOGGING_MEMLEAK_CREATE

		virtual String& GetDebugString() const;
//		virtual int GetHash() const;
	};

	template<typename T> class Pointer
	{
	private:
		T* value;
	
	public:
		static Pointer< T > Null;
		virtual ~Pointer() { }

		Pointer() : value(NULL) { }
		Pointer(const Pointer< T >& t) : value(t.value) { }
		Pointer(T* t) : value(t) { }
		Pointer(T& t) : value(&t) { }
		template<typename U> Pointer(const Pointer< U >& t) : value(dynamic_cast<T*>(t.GetValue())) { }
		template<typename U> Pointer(const U& t, typename SFINAE::EnableIf<!SFINAE::IsConst<U>::Value && SFINAE::IsSameType<T, U>::Value>::Type* dummy = NULL);

		Pointer< T >& operator=(const Pointer< T >& t) { Swap(t); return *this; }
		Pointer< T >& operator=(T& t) { value = &t; return *this; }
		T* operator->() const { return &**this; }
		T& operator*() const;
#ifdef BRICKS_CONFIG_CPP0X
		explicit operator T*() const { return value; }
#endif
		T* GetValue() const { return value; }
		operator T&() const { return **this; }
		operator bool() const { return value; }

		void Swap(const Pointer< T >& t) { value = t.value; }
	};
	template<typename T> Pointer< T > Pointer< T >::Null = Pointer< T >(NULL);
	
	template<typename T, typename T2> static bool operator==(const Pointer< T >& t1, const Pointer< T2 >& t2) { return t1.GetValue() == t2.GetValue(); }
	template<typename T, typename T2> static bool operator!=(const Pointer< T >& t1, const Pointer< T2 >& t2) { return t1.GetValue() != t2.GetValue(); }

	template<typename T> class AutoPointer : public Pointer< T >
	{
	private:
		void Retain() { if (*this) dynamic_cast<Object*>(this->GetValue())->Retain(); }
		void Release() { if (*this) dynamic_cast<Object*>(this->GetValue())->Release(); }

	public:
		AutoPointer() { }
		AutoPointer(const AutoPointer< T >& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		AutoPointer(const Pointer< T >& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		AutoPointer(T* t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		AutoPointer(T& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		template<typename U> AutoPointer(const U& t, bool retain = true, typename SFINAE::EnableIf<!SFINAE::IsConst<U>::Value && SFINAE::IsSameType<T, U>::Value>::Type* dummy = NULL) : Pointer< T >(t) { if (retain) Retain(); }
		template<typename U> AutoPointer(const Pointer< U >& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		virtual ~AutoPointer() { Release(); }

		AutoPointer< T >& operator=(const Pointer< T >& t) { Swap(t); return *this; }
		AutoPointer< T >& operator=(const AutoPointer< T >& t) { Swap(t); return *this; }
		AutoPointer< T >& operator=(T* t) { Swap(t); return *this; }
		AutoPointer< T >& operator=(T& t) { Swap(t); return *this; }
		template<typename U> typename SFINAE::EnableIf<!SFINAE::IsConst<U>::Value && SFINAE::IsSameType<T, U>::Value, AutoPointer< T >&>::Type operator=(const U& t) { Swap(t); return *this; }
		template<typename U> AutoPointer< T >& operator=(const Pointer< U >& t) { Swap(t); return *this; }

		void Swap(const Pointer< T >& t, bool retain = true) { if (this->GetValue() == t.GetValue()) return; Release(); Pointer< T >::operator=(t); if (retain) Retain(); }
	};

	template<typename T> class CopyPointer : public AutoPointer< T >
	{
#define BRICKS_COPY_POINTER(t) ((t) ? &(t)->Copy() : NULL)
	public:
		CopyPointer() { }
		CopyPointer(const CopyPointer< T >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(t.GetValue()), false) { }
		CopyPointer(const Pointer< T >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(t.GetValue()), false) { }
		CopyPointer(const T* t) : AutoPointer< T >(BRICKS_COPY_POINTER(t), false) { }
		CopyPointer(const T& t) : AutoPointer< T >(BRICKS_COPY_POINTER(&t), false) { }
		template<typename U> CopyPointer(const Pointer< U >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(t.GetValue()), false) { }
		
		CopyPointer< T >& operator=(const Pointer< T >& t) { Swap(t); return *this; }
		CopyPointer< T >& operator=(const CopyPointer< T >& t) { Swap(t); return *this; }
		CopyPointer< T >& operator=(const T* t) { AutoPointer< T >::Swap(BRICKS_COPY_POINTER(t), false); return *this; }
		CopyPointer< T >& operator=(const T& t) { AutoPointer< T >::Swap(BRICKS_COPY_POINTER(&t), false); return *this; }
		template<typename U> CopyPointer< T >& operator=(const Pointer< U >& t) { Swap(t); return *this; }

		void Swap(const Pointer< T >& t) { AutoPointer< T >::Swap(BRICKS_COPY_POINTER(t.GetValue()), false); }
#undef BRICKS_COPY_POINTER
	};
}

#include "bricks/alloc.h"
#include "bricks/objectpool.h"
#include "bricks/string.h"
#include "bricks/class.h"
#include "bricks/exception.h"

namespace Bricks {
	class ObjectPoolLeakException : public Exception
	{
	public:
		ObjectPoolLeakException(const String& message = String::Empty) : Exception(message) { }
	};

	template<typename T> inline T& Pointer< T >::operator*() const { if (!value) throw NullReferenceException(); return *value; }
#ifdef BRICKS_CONFIG_RTTI
	inline String& Object::GetDebugString() const { return String::Format("%s <%p> [%d]", GetClass().GetName().CString(), this, GetReferenceCount()); }
#else
	inline String& Object::GetDebugString() const { return String::Format("<%p> [%d]", this, GetReferenceCount()); }
#endif

	typedef Pointer<Object> any;
	
	inline void* Object::mallocthrowable(size_t size)
	{
		void* data = malloc(size);
		if (!data)
			throw OutOfMemoryException();
		return data;
	}
	template<typename T> template<typename U> inline Pointer< T >::Pointer(const U& t, typename SFINAE::EnableIf<!SFINAE::IsConst<U>::Value && SFINAE::IsSameType<T, U>::Value>::Type* dummy) : value(&const_cast<U&>(t)) {
		throw InvalidArgumentException();
	}
}
