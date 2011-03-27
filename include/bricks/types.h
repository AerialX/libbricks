#pragma once

#define BRICKS_VERSION_MAJOR 0
#define BRICKS_VERISON_MINOR 1
#define BRICKS_VERSION "0.1"

#include <stdint.h>

typedef int8_t		s8;
typedef uint8_t		u8;
typedef int16_t		s16;
typedef uint16_t	u16;
typedef int32_t		s32;
typedef uint32_t	u32;
typedef int64_t		s64;
typedef uint64_t	u64;

typedef float		f32;
typedef double		f64;

#include "bricks/config.h"

/* Features */
#define BRICKS_FEATURE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define BRICKS_FEATURE_MAX(a, b) ((a) > (b) ? (a) : (b))
#define BRICKS_FEATURE_ROUND_DOWN(num, round) ((num) / (round) * (round))
#define BRICKS_FEATURE_ROUND_UP(num, round) (BRICKS_FEATURE_ROUND_DOWN((num) + (round) - 1, round))

#ifdef _MSC_VER
#define BRICKS_FEATURE_VCPP
#endif
#ifdef __GNUC__
#define BRICKS_FEATURE_GCC
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 3
#define BRICKS_FEATURE_GCC_4_3
#endif
#endif

#ifdef BRICKS_FEATURE_GCC
#define BRICKS_FEATURE_NORETURN __attribute__((noreturn))
#define BRICKS_FEATURE_CONSTRUCTOR(function) __attribute__((constructor)) static void function()
#else
struct BRICKS_FEATURE_CONSTRUCTOR { BRICKS_FEATURE_CONSTRUCTOR(void (*function)()) { function(); } };
#define BRICKS_FEATURE_CONSTRUCTOR(function) void function(); static BRICKS_FEATURE_CONSTRUCTOR constructor_##function(function)
#ifdef BRICKS_FEATURE_VCPP
#define BRICKS_FEATURE_NORETURN __declspec(noreturn)
#else
#define BRICKS_FEATURE_NORETURN
#endif
#endif

#ifdef BRICKS_CONFIG_LOGGING
#include <stdio.h>
#define BRICKS_FEATURE_LOG(...) if (true) { fprintf(stderr, "[BRICKS] " __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr); } else (void)0
#else
#define BRICKS_FEATURE_LOG(...)
#endif
#ifdef BRICKS_CONFIG_LOGGING_HEAVY
#define BRICKS_FEATURE_LOG_HEAVY BRICKS_FEATURE_LOG
#else
#define BRICKS_FEATURE_LOG_HEAVY(...)
#endif
