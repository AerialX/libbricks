#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

#include <stdlib.h>

#ifdef BRICKS_CONFIG_RTTI
#include <typeinfo>
#endif

#define self (*this)
#define id Pointer<Object>
#define nil NULL

namespace Bricks {
	class Class;
	class String;

	class Object
	{
	private:
		int referenceCount;

#ifdef BRICKS_CONFIG_CPP0X
		template<typename T, typename... Args> friend T& Alloc(Args... args);
#endif

	public:
		Object() : referenceCount(-0x40000000) { }

#ifdef BRICKS_CONFIG_RTTI
		Class& GetClass() const;
		template<class T> bool IsSubclassOf() const { return dynamic_cast< T >(this); }
		template<class T> bool IsClassOf() const { return typeid(this) == typeid(T); }
#endif

		Object& Autorelease();
		Object& Retain() { if (referenceCount > 0) referenceCount++; BRICKS_FEATURE_LOG_HEAVY("+%p [%d]", this, referenceCount); return self; }
		void Release() { BRICKS_FEATURE_LOG_HEAVY("-%p [%d]", this, referenceCount - 1); if (referenceCount > 0 && !--referenceCount) delete this; }
		int GetReferenceCount() const { return referenceCount; }
		template<typename T> T& Autorelease() { return static_cast<T&>(Autorelease()); }
		template<typename T> T& Retain() { return static_cast<T&>(Retain()); }
		const Object& Autorelease() const { return const_cast<Object*>(this)->Autorelease(); }
		const Object& Retain() const { return const_cast<Object*>(this)->Retain(); }
		void Release() const { const_cast<Object*>(this)->Release(); }
		virtual ~Object() { }

		virtual bool operator==(const Object& rhs) const { return this == &rhs; }
		virtual bool operator!=(const Object& rhs) const { return !operator==(rhs); }
		
		static Object& AllocInternal(Object& object) { object.referenceCount = 1; return object; }
		
		virtual String& GetDebugString() const;
//		virtual int GetHash() const;
	};

	class Copyable
	{
	public:
		virtual Object& Copy() const = 0;
		template<typename T> T& Copy() const { return static_cast<T&>(Copy()); }
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

		bool operator==(const Pointer< T >& t) { return t.value == value; }
		bool operator!=(const Pointer< T >& t) const { return !operator==(t); }
/*		bool operator==(const T* t) const { return t == value; }
		bool operator!=(const T* t) const { return !operator==(t); }
		bool operator==(const Pointer< T >& t) { return (value && t.value) ? (*t.value == *value) : false; }
		bool operator!=(const Object& t) const { return !operator==(t); }
		bool operator==(const Object& t) { return value ? t == *value : false; }*/

		Pointer< T >& operator=(const Pointer< T >& t) { value = t.value; return self; }
		Pointer< T >& operator=(T* t) { value = t; return self; }
		Pointer< T >& operator=(T& t) { value = &t; return self; }
		T* operator->() { return value; }
		const T* operator->() const { return value; }
		T& operator*();
		const T& operator*() const;
		explicit operator T*() { return value; }
		explicit operator T*() const { return value; }
		operator T&() { return *self; }
		operator T&() const { return *self; }
		operator bool() const { return value; }
	};
	template<typename T> Pointer< T > Pointer< T >::Null = Pointer< T >(NULL);

	template<typename T> class AutoPointer : public Pointer< T >
	{
	private:
		void Retain() { if (self) dynamic_cast<Object*>(static_cast<T*>(self))->Retain(); }
		void Release() { if (self) dynamic_cast<Object*>(static_cast<T*>(self))->Release(); }

	public:
		AutoPointer() : Pointer< T >() { }
		AutoPointer(const AutoPointer< T >& t) : Pointer< T >(t) { Retain(); }
		AutoPointer(const Pointer< T >& t) : Pointer< T >(t) { Retain(); }
		AutoPointer(T* t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		AutoPointer(T& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		~AutoPointer() { Release(); }
		
		AutoPointer< T >& operator=(const AutoPointer<T>& t) { if (this == &t) return self; Release(); Pointer< T >::operator=(t); Retain(); return self; }
		AutoPointer< T >& operator=(T* t) { if (&dynamic_cast<Object&>(*self) == dynamic_cast<Object*>(t)) return self; Release(); Pointer< T >::operator=(t); Retain(); return self; }
		AutoPointer< T >& operator=(T& t) { if (&dynamic_cast<Object&>(*self) == &dynamic_cast<Object&>(t)) return self; Release(); Pointer< T >::operator=(t); Retain(); return self; }
	};

	template<typename T> class CopyPointer : public AutoPointer< T >
	{
#define BRICKS_COPY_POINTER(t) (static_cast<const Copyable*>(t)->Copy< T >())
	public:
		CopyPointer() : AutoPointer< T >() { }
		CopyPointer(const CopyPointer< T >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(&*t), false) { }
		CopyPointer(const Pointer< T >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(&*t), false) { }
		CopyPointer(const T* t) : AutoPointer< T >(BRICKS_COPY_POINTER(t), false) { }
		CopyPointer(const T& t) : AutoPointer< T >(BRICKS_COPY_POINTER(&t), false) { }
		
		CopyPointer< T >& operator=(const Pointer<T>& t) { AutoPointer< T >::operator=(BRICKS_COPY_POINTER(&*t)); return self; }
		CopyPointer< T >& operator=(const T* t) { AutoPointer< T >::operator=(BRICKS_COPY_POINTER(t)); return self; }
		CopyPointer< T >& operator=(const T& t) { AutoPointer< T >::operator=(BRICKS_COPY_POINTER(&*t)); return self; }
	};

#ifdef BRICKS_CONFIG_CPP0X
	template<typename T, typename... Args> T& Alloc(Args... args)
	{
		T* object = new T(args...);
		object->referenceCount = 1;
		return *object;
	}

	template<typename T, typename... Args> T& AutoAlloc(Args... args)
	{
		return static_cast<Object&>(Alloc< T >(args...)).Autorelease< T >();
	}
#endif
#define Alloc(T, ...) (static_cast<T&>(Bricks::Object::AllocInternal(*new T(__VA_ARGS__))))
#define AutoAlloc(T, ...) (static_cast<Object&>(Alloc(T, __VA_ARGS__)).Autorelease< T >())

	template<typename T> class CopyableConstructor : public Copyable
	{
	public:
		virtual Object& Copy() const { return Alloc(T, static_cast<const T&>(self)); }
	};
}

#include "bricks/objectpool.h"
#include "bricks/string.h"
#include "bricks/class.h"
#include "bricks/exception.h"

namespace Bricks {
	class ObjectPoolLeakException : public Object
	{
	public:
		ObjectPoolLeakException() { }
	};

	template<typename T> inline T& Pointer< T >::operator*() { if (!value) Throw(NullReferenceException); return *value; }
	template<typename T> inline const T& Pointer< T >::operator*() const { if (!value) Throw(NullReferenceException); return *value; }
#ifdef BRICKS_CONFIG_RTTI
	inline String& Object::GetDebugString() const { return String::Format("%s <%p> [%d]", GetClass().GetName().CString(), this, referenceCount); }
#else
	inline String& Object::GetDebugString() const { return String::Format("<%p> [%d]", this, referenceCount); }
#endif
}
