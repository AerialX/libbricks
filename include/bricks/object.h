#pragma once

#include "bricks/types.h"
#include <stdlib.h>
#include <typeinfo>

#define self (*this)

namespace Bricks {
	class Class;

	class Object
	{
	private:
		int referenceCount;

		template<typename T, typename... Args> friend T& Alloc(Args... args);

	public:
		Object() : referenceCount(-0x40000000) { }

#ifdef BRICKS_CONFIG_RTTI
		Class& GetClass() const;
		template<class T> bool IsSubclassOf() const { return dynamic_cast<T>(this); }
		template<class T> bool IsClassOf() const { return typeid(this) == typeid(T); }
#endif

		Object& Autorelease();
		Object& Retain() { referenceCount++; return self; }
		void Release() { if (!--referenceCount) delete this; }
		int GetReferenceCount() const { return referenceCount; }
		virtual ~Object() { }

		virtual bool operator==(const Object& rhs) const { return this == &rhs; }

		/* Garbage
		virtual int GetHash() const;
		virtual String& GetDebug() const;
		*/
	};
	
	template<typename T, typename... Args> T& Alloc(Args... args)
	{
		T* object = new T(args...);
		object->referenceCount = 1;
		return *object;
	}
}
