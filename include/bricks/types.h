#pragma once

#define BRICKS_VERSION_MAJOR 0
#define BRICKS_VERISON_MINOR 1
#define BRICKS_VERSION "0.1"

#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
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

typedef volatile int8_t		vs8;
typedef volatile uint8_t	vu8;
typedef volatile int16_t	vs16;
typedef volatile uint16_t	vu16;
typedef volatile int32_t	vs32;
typedef volatile uint32_t	vu32;
typedef volatile int64_t	vs64;
typedef volatile uint64_t	vu64;
typedef volatile float		vf32;
typedef volatile double		vf64;

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
#ifdef __OBJC__
#define BRICKS_FEATURE_OBJC
#endif
#ifdef __APPLE__
#define BRICKS_FEATURE_APPLE
#endif

/* Attributes */
#ifdef BRICKS_FEATURE_GCC
#define BRICKS_FEATURE_NORETURN __attribute__((noreturn))
#define BRICKS_FEATURE_CONSTRUCTOR(function) __attribute__((constructor))
#define BRICKS_FEATURE_DESTRUCTOR(function) __attribute__((destructor))
#define BRICKS_FEATURE_TLS __thread
#else
struct BRICKS_FEATURE_CONSTRUCTOR { BRICKS_FEATURE_CONSTRUCTOR(void (*function)()) { function(); } };
struct BRICKS_FEATURE_DESTRUCTOR { void (*function)(); BRICKS_FEATURE_DESTRUCTOR(void (*function)()) : function(function) { } ~BRICKS_FEATURE_DESTRUCTOR() { function(); } };
#define BRICKS_FEATURE_CONSTRUCTOR(function) void function(); static BRICKS_FEATURE_CONSTRUCTOR constructor_##function(function);
#define BRICKS_FEATURE_DESTRUCTOR(function) void function(); static BRICKS_FEATURE_DESTRUCTOR constructor_##function(function);
#ifdef BRICKS_FEATURE_VCPP
#define BRICKS_FEATURE_NORETURN __declspec(noreturn)
#define BRICKS_FEATURE_TLS __declspec(thread)
#else
#define BRICKS_FEATURE_NORETURN
#endif
#endif

/* Logging */
#ifdef BRICKS_CONFIG_LOGGING
#include <stdio.h>
#define BRICKS_FEATURE_LOG(...) do { fprintf(stderr, "[BRICKS] " __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr); } while (false)
#else
#define BRICKS_FEATURE_LOG(...)
#endif
#ifdef BRICKS_CONFIG_LOGGING_HEAVY
#define BRICKS_FEATURE_LOG_HEAVY BRICKS_FEATURE_LOG
#else
#define BRICKS_FEATURE_LOG_HEAVY(...)
#endif
