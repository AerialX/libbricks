#pragma once

#include "bricks.h"
#include "bricks/collections.h"
#include "bricks/collections/array.h"

#define BRICKS_ARGLIST_HEADER "bricks/delegate_template.h"

#include "bricks/internal/arglist.h"

#undef BRICKS_ARGLIST_HEADER

namespace Bricks { namespace Collections {
	template<typename T> inline void Iterable< T >::Iterate(const Delegate<bool(T&)>& delegate) {
		foreach (T& t, *this) {
			if (!const_cast<Delegate<bool(T&)>&>(delegate).Call(t))
				break;
		}
	}
} }
