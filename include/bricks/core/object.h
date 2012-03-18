#pragma once

#include "bricks/core/types.h"

#include <stdlib.h>

namespace Bricks {
	class Object;
	class String;

	namespace Internal {
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

	protected:
		Object(const Object& object) : referenceCount(object.referenceCount) { }
		Object& operator =(const Object& object) { referenceCount = object.referenceCount; return *this; }

	public:
		Object() { BRICKS_FEATURE_LOG_HEAVY("> %p [%d]", this, GetReferenceCount());}
		virtual ~Object() { BRICKS_FEATURE_LOG_HEAVY("< %p [%d]", this, GetReferenceCount()); }

#if BRICKS_ENV_RELEASE
		void Retain() { referenceCount++; BRICKS_FEATURE_LOG_HEAVY("+ %p [%d]", this, GetReferenceCount()); }
		void Release() { BRICKS_FEATURE_LOG_HEAVY("- %p [%d]", this, GetReferenceCount() - 1); if (!--referenceCount) delete this; }
#else
		void Retain();
		void Release();
#endif

		int GetReferenceCount() const { return referenceCount; }

		virtual bool operator ==(const Object& rhs) const { return this == &rhs; }
		virtual bool operator !=(const Object& rhs) const { return this != &rhs; }

		void* operator new(size_t size) { return malloc(size); }
		void operator delete(void* data) { free(data); }

		virtual String GetDebugString() const;
//		virtual int GetHash() const;
	};
}
