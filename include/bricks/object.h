#pragma once

#include "bricks/types.h"
#include "bricks/sfinae.h"

#include <stdlib.h>

#ifdef BRICKS_CONFIG_RTTI
#include <typeinfo>
#endif

#define BRICKS_COPY_CONSTRUCTOR(T) public: virtual AutoPointer< T > Copy() const { return autonew T(*this); }
#define BRICKS_COPY_HIDE(T) protected: T(const T&); T& operator=(const T&);

#define autonew Bricks::Internal::AutoAllocPointer() * new

namespace Bricks {
	class Class;
	class String;
	class Object;
	template<typename T> class Pointer;
	template<typename T> class AutoPointer;
	template<typename T> class CopyPointer;
	template<typename T> class ReturnPointer;

	namespace Internal {
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

	protected:
		Object(const Object& object) : referenceCount(object.referenceCount) { }
		Object& operator =(const Object& object) { referenceCount = object.referenceCount; return *this; }

	public:
		Object() { BRICKS_FEATURE_LOGGING_MEMLEAK(); BRICKS_FEATURE_LOG_HEAVY("> %p [%d]", this, GetReferenceCount());}
		virtual ~Object() { BRICKS_FEATURE_LOG_HEAVY("< %p [%d]", this, GetReferenceCount()); }

#ifdef BRICKS_CONFIG_RTTI
		ReturnPointer<Class> GetClass() const;
		template<class T> bool IsSubclassOf() const { return dynamic_cast< T >(this); }
		template<class T> bool IsClassOf() const { return typeid(this) == typeid(T); }
#endif

		void Retain();
		void Release();
		int GetReferenceCount() const { return referenceCount; }

		virtual bool operator==(const Object& rhs) const { return this == &rhs; }
		virtual bool operator!=(const Object& rhs) const { return this != &rhs; }

		void* operator new(size_t size, const Internal::ObjectGlobalAlloc& dummy) { return mallocthrowable(size); }
		void* operator new(size_t size) { BRICKS_FEATURE_LOGGING_MEMLEAK_CREATE(); return mallocthrowable(size); }
		void operator delete(void* data) { free(data); }

		virtual String GetDebugString() const;
//		virtual int GetHash() const;
	};

	template<typename T> class Pointer
	{
	private:
		T* value;
	
	public:
		typedef T Type;

		static Pointer< T > Null;

		Pointer() : value(NULL) { }
		Pointer(const Pointer< T >& t) : value(t.value) { }
		Pointer(T* t) : value(t) { }
		Pointer(T& t) : value(&t) { }
		template<typename U> Pointer(const Pointer< U >& t, typename SFINAE::EnableIf<SFINAE::IsCompatibleType<T, U>::Value>::Type* dummy = NULL) : value(static_cast<T*>(t.GetValue())) { }
		template<typename U> Pointer(const Pointer< U >& t, typename SFINAE::EnableIf<!SFINAE::IsCompatibleType<T, U>::Value>::Type* dummy = NULL) : value(dynamic_cast<T*>(t.GetValue())) { }
		template<typename U> Pointer(const U& t, typename SFINAE::EnableIf<!SFINAE::IsConst<U>::Value && SFINAE::IsSameType<T, U>::Value>::Type* dummy = NULL);

		Pointer< T >& operator=(const Pointer< T >& t) { Swap(t); return *this; }
		Pointer< T >& operator=(T* t) { value = t; return *this; }
		Pointer< T >& operator=(T& t) { value = &t; return *this; }
		T* operator->() const { return &**this; }
		T& operator*() const;
		T* GetValue() const { return value; }
		operator T&() const { return **this; }
		operator bool() const { return value; }

		void Swap(const Pointer< T >& t) { value = t.value; }

		bool operator==(const Pointer< T >& t) const { return value == t.value; }
		bool operator!=(const Pointer< T >& t) const { return value != t.value; }
		bool operator>(const Pointer< T >& t) const { return value > t.value; }
		bool operator<(const Pointer< T >& t) const { return value < t.value; }
		template<typename U> bool operator==(const Pointer< U >& t) const { return value == t.GetValue(); }
		template<typename U> bool operator!=(const Pointer< U >& t) const { return value != t.GetValue(); }

		template<typename U> bool IsType() const { return AsType<U>(); }

		template<typename U> Pointer<U> AsType(typename SFINAE::EnableIf<SFINAE::IsCompatibleType<U, T>::Value>::Type* dummy = NULL) const { if (!value) return Pointer<U>::Null; return static_cast<U*>(value); }
		template<typename U> Pointer<U> AsType(typename SFINAE::EnableIf<!SFINAE::IsCompatibleType<U, T>::Value>::Type* dummy = NULL) const { if (!value) return Pointer<U>::Null; return dynamic_cast<U*>(value); }
	};
	template<typename T> Pointer< T > Pointer< T >::Null = Pointer< T >(NULL);
	
	template<typename T, typename T2> static bool operator==(const Pointer< T >& t1, const Pointer< T2 >& t2) { return t1.GetValue() == t2.GetValue(); }
	template<typename T, typename T2> static bool operator!=(const Pointer< T >& t1, const Pointer< T2 >& t2) { return t1.GetValue() != t2.GetValue(); }

	template<typename T> class AutoPointer : public Pointer< T >
	{
	private:
		template<typename U> static void Retain(const Pointer<U>& ptr) { if (ptr) ptr.template AsType<Object>()->Retain(); }
		template<typename U> static void Release(const Pointer<U>& ptr) { if (ptr) ptr.template AsType<Object>()->Release(); }

	public:
		AutoPointer() { }
		AutoPointer(const AutoPointer< T >& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(*this); }
		AutoPointer(const Pointer< T >& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(*this); }
		AutoPointer(T* t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(*this); }
		AutoPointer(T& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(*this); }
		template<typename U> AutoPointer(const U& t, bool retain = true, typename SFINAE::EnableIf<!SFINAE::IsConst<U>::Value && SFINAE::IsSameType<T, U>::Value>::Type* dummy = NULL) : Pointer< T >(t) { if (retain) Retain(*this); }
		template<typename U> AutoPointer(const Pointer< U >& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(*this); }
		~AutoPointer() { Release(*this); }

		AutoPointer< T >& operator=(const Pointer< T >& t) { Swap(t); return *this; }
		AutoPointer< T >& operator=(const AutoPointer< T >& t) { Swap(t); return *this; }
		AutoPointer< T >& operator=(T* t) { Swap(t); return *this; }
		AutoPointer< T >& operator=(T& t) { Swap(t); return *this; }
		template<typename U> typename SFINAE::EnableIf<!SFINAE::IsConst<U>::Value && SFINAE::IsSameType<T, U>::Value, AutoPointer< T >&>::Type operator=(const U& t) { Swap(t); return *this; }
		template<typename U> AutoPointer< T >& operator=(const Pointer< U >& t) { Swap(t); return *this; }

		void Swap(const Pointer< T >& t, bool retain = true) { if (this->GetValue() == t.GetValue()) return; Release(*this); Pointer< T >::Swap(t); if (retain) Retain(*this); }
		void Release() { Release(*this); Pointer<T>::Swap(NULL); }
	};

	template<typename T> class ReturnPointer : public AutoPointer< T >
	{
	private:
		bool retained;

	public:
		ReturnPointer() : retained(false) { }
		ReturnPointer(const ReturnPointer< T >& t) : AutoPointer< T >(t, t.retained), retained(t.retained) { }
		ReturnPointer(const AutoPointer< T >& t, bool retain = true) : AutoPointer< T >(t, retain), retained(retain) { }
		template<typename U> ReturnPointer(const Pointer< U >& t, bool retain = true) : AutoPointer< T >(t, retain), retained(retain) { }
		ReturnPointer(const Pointer< T >& t, bool retain = false) : AutoPointer< T >(t, retain), retained(retain) { }
		ReturnPointer(T* t, bool retain = false) : AutoPointer< T >(t, retain), retained(retain) { }
		ReturnPointer(T& t, bool retain = false) : AutoPointer< T >(t, retain), retained(retain) { }

		~ReturnPointer() { if (!retained && *this) Pointer<T>::Swap(NULL); }

		ReturnPointer< T >& operator=(const Pointer< T >& t) { Swap(t, false); return *this; }
		ReturnPointer< T >& operator=(const AutoPointer< T >& t) { Swap(t, true); return *this; }
		ReturnPointer< T >& operator=(const ReturnPointer< T >& t) { Swap(t, t.retained); return *this; }
		ReturnPointer< T >& operator=(T* t) { Swap(t, false); return *this; }
		ReturnPointer< T >& operator=(T& t) { Swap(t, false); return *this; }
		template<typename U> ReturnPointer< T >& operator=(const Pointer< U >& t) { Swap(t, false); return *this; }
		template<typename U> ReturnPointer< T >& operator=(const AutoPointer< U >& t) { Swap(t, true); return *this; }

		void Swap(const Pointer< T >& t, bool retain = true) { if (!retained && *this) Pointer< T >::Swap(NULL); AutoPointer< T >::Swap(t, retain); retained = retain; }
		void Release() { if (retained) Release(*this); Pointer<T>::Swap(NULL); }

		template<typename U> ReturnPointer<U> AsType() const { if (*this) return ReturnPointer<U>(Pointer<T>::template AsType<U>(), retained); return Pointer<U>::Null; }
	};

	namespace Internal {
		struct AutoAllocPointer
		{
			template<typename U> typename SFINAE::EnableIf<SFINAE::IsCompatibleType<Object, U>::Value, AutoPointer<U> >::Type operator *(U* t) { return AutoPointer<U>(t, false); }
		};
	}

	template<typename T> class CopyPointer : public AutoPointer< T >
	{
#define BRICKS_COPY_POINTER(t) ((t) ? (t)->Copy() : Pointer<T>::Null)
#define BRICKS_COPY_REFERENCE(t) ((t).Copy())
	public:
		CopyPointer() { }
		CopyPointer(const CopyPointer< T >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(t)) { }
		CopyPointer(const Pointer< T >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(t)) { }
		CopyPointer(const T* t) : AutoPointer< T >(BRICKS_COPY_POINTER(t)) { }
		CopyPointer(const T& t) : AutoPointer< T >(BRICKS_COPY_REFERENCE(t)) { }
		template<typename U> CopyPointer(const Pointer< U >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(t)) { }

		CopyPointer< T >& operator=(const Pointer< T >& t) { Swap(t); return *this; }
		CopyPointer< T >& operator=(const CopyPointer< T >& t) { Swap(t); return *this; }
		CopyPointer< T >& operator=(const T* t) { AutoPointer< T >::Swap(BRICKS_COPY_POINTER(t)); return *this; }
		CopyPointer< T >& operator=(const T& t) { AutoPointer< T >::Swap(BRICKS_COPY_REFERENCE(&t)); return *this; }
		template<typename U> CopyPointer< T >& operator=(const Pointer< U >& t) { Swap(t); return *this; }

		void Swap(const Pointer< T >& t) { AutoPointer< T >::Swap(BRICKS_COPY_POINTER(t)); }
#undef BRICKS_COPY_POINTER
	};

	class NoCopy { private: NoCopy(const NoCopy&); NoCopy& operator =(const NoCopy&); protected: NoCopy() { } };
}

#include "bricks/internal/string.h"
#include "bricks/internal/class.h"
#include "bricks/internal/value.h"
#include "bricks/internal/data.h"
#include "bricks/internal/exception.h"

namespace Bricks {
	class ObjectPoolLeakException : public Exception
	{
	public:
		ObjectPoolLeakException(const String& message = String::Empty) : Exception(message) { }
	};

#ifdef BRICKS_FEATURE_RELEASE
	template<typename T> inline T& Pointer< T >::operator*() const { return *value; }
#else
	template<typename T> inline T& Pointer< T >::operator*() const { if (!value) throw NullReferenceException(); return *value; }
#endif

#ifdef BRICKS_CONFIG_RTTI
	inline ReturnPointer<Class> Object::GetClass() const { return autonew Class(this); }
	inline String Object::GetDebugString() const { return String::Format("%s <%p> [%d]", GetClass()->GetName().CString(), this, GetReferenceCount()); }
#else
	inline String Object::GetDebugString() const { return String::Format("<%p> [%d]", this, GetReferenceCount()); }
#endif

	inline void Object::Retain() { referenceCount++; BRICKS_FEATURE_LOG_HEAVY("+ %p [%d]", this, GetReferenceCount()); }
	inline void Object::Release() { BRICKS_FEATURE_LOG_HEAVY("- %p [%d]", this, GetReferenceCount() - 1); BRICKS_FEATURE_ASSERT(referenceCount > 0); if (!--referenceCount) delete this; BRICKS_FEATURE_LOGGING_MEMLEAK_DESTROY(); }

#undef BRICKS_FEATURE_LOGGING_MEMLEAK
#undef BRICKS_FEATURE_LOGGING_MEMLEAK_DESTROY
#undef BRICKS_FEATURE_LOGGING_MEMLEAK_CREATE
	
	inline void* Object::mallocthrowable(size_t size)
	{
		void* data = malloc(size);
#ifndef BRICKS_FEATURE_RELEASE
		if (!data)
			throw OutOfMemoryException();
#endif
		return data;
	}
	template<typename T> template<typename U> inline Pointer< T >::Pointer(const U& t, typename SFINAE::EnableIf<!SFINAE::IsConst<U>::Value && SFINAE::IsSameType<T, U>::Value>::Type* dummy) : value(&const_cast<U&>(t)) { throw InvalidArgumentException(); }
}
