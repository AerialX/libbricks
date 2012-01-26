#pragma once

#include "bricks/core/types.h"

#include <stdlib.h>

namespace Bricks {
	class Object;
	class String;

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

	protected:
		Object(const Object& object) : referenceCount(object.referenceCount) { }
		Object& operator =(const Object& object) { referenceCount = object.referenceCount; return *this; }

	public:
		Object() { BRICKS_FEATURE_LOGGING_MEMLEAK(); BRICKS_FEATURE_LOG_HEAVY("> %p [%d]", this, GetReferenceCount());}
		virtual ~Object() { BRICKS_FEATURE_LOG_HEAVY("< %p [%d]", this, GetReferenceCount()); }

#ifdef BRICKS_FEATURE_RELEASE
		void Retain() { referenceCount++; BRICKS_FEATURE_LOG_HEAVY("+ %p [%d]", this, GetReferenceCount()); }
		void Release() { BRICKS_FEATURE_LOG_HEAVY("- %p [%d]", this, GetReferenceCount() - 1); if (!--referenceCount) delete this; BRICKS_FEATURE_LOGGING_MEMLEAK_DESTROY(); }
#else
		void Retain();
		void Release();
#endif

		int GetReferenceCount() const { return referenceCount; }

		virtual bool operator ==(const Object& rhs) const { return this == &rhs; }
		virtual bool operator !=(const Object& rhs) const { return this != &rhs; }

		void* operator new(size_t size, const Internal::ObjectGlobalAlloc& dummy) { return malloc(size); }
		void* operator new(size_t size) { BRICKS_FEATURE_LOGGING_MEMLEAK_CREATE(); return malloc(size); }
		void operator delete(void* data) { free(data); }

		virtual String GetDebugString() const;
//		virtual int GetHash() const;
	};
}
