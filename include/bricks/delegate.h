#pragma once

#include "bricks/object.h"

#define BRICKS_ARGLIST_HEADER "bricks/internal/delegate.h"
#define BRICKS_ARGLIST_TYPES_NAMES BRICKS_ARGLIST_ENUM2(BRICKS_ARGLIST_COUNT, T, arg)
#define BRICKS_ARGLIST_ARGS BRICKS_ARGLIST_ENUM(BRICKS_ARGLIST_COUNT, arg)

#include "bricks/internal/arglist.h"

#undef BRICKS_ARGLIST_HEADER

#define BRICKS_ARGLIST_HEADER "bricks/internal/event.h"
#define BRICKS_ARGLIST_TYPES_NAMES BRICKS_ARGLIST_ENUM2(BRICKS_ARGLIST_COUNT, T, arg)
#define BRICKS_ARGLIST_ARGS BRICKS_ARGLIST_ENUM(BRICKS_ARGLIST_COUNT, arg)

#include "bricks/internal/arglist.h"

#undef BRICKS_ARGLIST_HEADER
