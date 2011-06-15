#pragma once

#include "bricks.h"
#include "bricks/collections.h"
#include "bricks/collections/stack.h"

#define BRICKS_HEADER_DELEGATE

#define BRICKS_DELEGATE_COUNT 0
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 1
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 2
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 3
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 4
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 5
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 6
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 7
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 8
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT
#define BRICKS_DELEGATE_COUNT 9
#include "bricks/delegate_template.h"
#undef BRICKS_DELEGATE_COUNT

namespace Bricks { namespace Collections {
	template<typename T> inline void Iterable< T >::Iterate(const Delegate<bool(T&)>& delegate) {
		foreach (T& t, self) {
			if (!const_cast<Delegate<bool(T&)>&>(delegate).Call(t))
				break;
		}
	}
} }
