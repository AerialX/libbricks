#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

#include <stdlib.h>

#ifdef BRICKS_CONFIG_RTTI
#include <typeinfo>
#endif

#define self (*this)
#define nil NULL
#define autoalloc *new(Bricks::Auto)
#define alloc *new

namespace Bricks {
	class Class;
	class String;

	struct ObjectAlloc { };
	extern ObjectAlloc Auto;

	class Object
	{
	private:
		u32 referenceCount;

		void InternalAutorelease();

		static void* mallocthrowable(size_t size);

	public:
		Object() : referenceCount(1) { }

#ifdef BRICKS_CONFIG_RTTI
		Class& GetClass() const;
		template<class T> bool IsSubclassOf() const { return dynamic_cast< T >(this); }
		template<class T> bool IsClassOf() const { return typeid(this) == typeid(T); }
#endif

		Object& Autorelease();
		Object& Retain() { referenceCount++; BRICKS_FEATURE_LOG_HEAVY("+%p [%d]", this, referenceCount); return self; }
		void Release() { BRICKS_FEATURE_LOG_HEAVY("-%p [%d]", this, referenceCount - 1); if (!--referenceCount) delete this; }
		int GetReferenceCount() const { return referenceCount; }
		template<typename T> T& Autorelease() { return static_cast<T&>(Autorelease()); }
		template<typename T> T& Retain() { return static_cast<T&>(Retain()); }
		virtual ~Object() { }

		virtual bool operator==(const Object& rhs) const { return this == &rhs; }
		virtual bool operator!=(const Object& rhs) const { return this != &rhs; }

		void* operator new(size_t size, const ObjectAlloc& dummy) {
			Object* obj = reinterpret_cast<Object*>(mallocthrowable(size));
			obj->referenceCount = UINT32_MAX / 2;
			obj->InternalAutorelease();
			return obj;
		}
		void* operator new(size_t size) { return mallocthrowable(size); }
		void operator delete(void* data) { free(data); }

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

		bool operator==(const Pointer< T >& t) { return t.value == value; }
		bool operator!=(const Pointer< T >& t) const { return t.value != value; }

		Pointer< T >& operator=(const Pointer< T >& t) { Swap(t); return self; }
		Pointer< T >& operator=(T& t) { value = &t; return self; }
		T* operator->() { return &*self; }
		const T* operator->() const { return &*self; }
		T& operator*();
		const T& operator*() const;
		explicit operator T*() { return value; }
		explicit operator T*() const { return value; }
		operator T&() { return *self; }
		operator T&() const { return *self; }
		operator bool() const { return value; }

		void Swap(const Pointer< T >& t) { value = t.value; }
	};
	template<typename T> Pointer< T > Pointer< T >::Null = Pointer< T >(NULL);

	template<typename T> class AutoPointer : public Pointer< T >
	{
	private:
		void Retain() { if (self) dynamic_cast<Object*>(static_cast<T*>(self))->Retain(); }
		void Release() { if (self) dynamic_cast<Object*>(static_cast<T*>(self))->Release(); }

	public:
		AutoPointer() { }
		AutoPointer(const AutoPointer< T >& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		AutoPointer(T* t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		AutoPointer(T& t, bool retain = true) : Pointer< T >(t) { if (retain) Retain(); }
		virtual ~AutoPointer() { Release(); }

		AutoPointer< T >& operator=(const Pointer< T >& t) { Swap(t); return self; }
		AutoPointer< T >& operator=(const AutoPointer< T >& t) { Swap(t); return self; }

		void Swap(const Pointer< T >& t, bool retain = true) { if (static_cast<T*>(self) == static_cast<T*>(t)) return; Release(); Pointer< T >::operator=(t); if (retain) Retain(); }
	};

	template<typename T> class CopyPointer : public AutoPointer< T >
	{
#define BRICKS_COPY_POINTER(t) ((t) ? &(t)->Copy() : NULL)
	public:
		CopyPointer() { }
		CopyPointer(const CopyPointer< T >& t) : AutoPointer< T >(BRICKS_COPY_POINTER(static_cast<T*>(t)), false) { }
		CopyPointer(const T* t) : AutoPointer< T >(BRICKS_COPY_POINTER(t), false) { }
		CopyPointer(const T& t) : AutoPointer< T >(BRICKS_COPY_POINTER(&t), false) { }
		
		CopyPointer< T >& operator=(const Pointer< T >& t) { Swap(t); return self; }
		CopyPointer< T >& operator=(const CopyPointer< T >& t) { Swap(t); return self; }

		void Swap(const Pointer< T >& t) { AutoPointer< T >::Swap(BRICKS_COPY_POINTER(static_cast<T*>(t))); }
	};

#define BRICKS_COPY_CONSTRUCTOR(T) T& Copy() const { return alloc T(self); }
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

	template<typename T> inline T& Pointer< T >::operator*() { if (!value) throw NullReferenceException(); return *value; }
	template<typename T> inline const T& Pointer< T >::operator*() const { if (!value) throw NullReferenceException(); return *value; }
#ifdef BRICKS_CONFIG_RTTI
	inline String& Object::GetDebugString() const { return String::Format("%s <%p> [%d]", GetClass().GetName().CString(), this, referenceCount); }
#else
	inline String& Object::GetDebugString() const { return String::Format("<%p> [%d]", this, referenceCount); }
#endif

	typedef Pointer<Object> id;
	
	inline void* Object::mallocthrowable(size_t size)
	{
		void* data = malloc(size);
		if (!data)
			throw OutOfMemoryException();
		return data;
	}
}
