#pragma once

#include "bricks/config.h"

#ifndef __STDC_LIMIT_MACROS
	#define __STDC_LIMIT_MACROS 1
#endif
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS 1
#endif
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

#ifdef _MSC_VER
#define BRICKS_ENV_VCPP 1
#endif
#ifdef __GNUC__
#define BRICKS_ENV_GCC 1
#endif
#ifdef __clang__
#define BRICKS_ENV_CLANG 1
#endif
#ifdef __APPLE__
#define BRICKS_ENV_APPLE 1
#endif
#ifdef __MINGW32__
#define BRICKS_ENV_MINGW 1
#endif
#ifdef __linux__
#define BRICKS_ENV_LINUX 1
#endif
#ifdef __BSD__
#define BRICKS_ENV_BSD 1
#endif
#ifdef __OBJC__
#define BRICKS_ENV_OBJC 1
#if BRICKS_ENV_APPLE || BRICKS_ENV_CLANG
#define BRICKS_ENV_OBJC_BLOCKS 1
#endif
#endif

#ifdef ANDROID
#define BRICKS_ENV_ANDROID 1
#endif

#if BRICKS_ENV_LINUX || BRICKS_ENV_BSD
#define BRICKS_ENV_LINUXBSD 1
#endif

#if BRICKS_ENV_GCC && !BRICKS_ENV_MINGW
#define BRICKS_ENV_UNIX 1
#endif

#if BRICKS_ENV_MINGW || BRICKS_ENV_VCPP
#define BRICKS_ENV_WINDOWS 1
#endif

#ifdef __EXCEPTIONS
#define BRICKS_ENV_EXCEPTIONS 1
#endif

#ifdef NDEBUG
#define BRICKS_ENV_RELEASE 1
#endif
#ifdef DEBUG
#define BRICKS_ENV_DEBUG 1
#endif

#if !BRICKS_ENV_DEBUG && !BRICKS_ENV_RELEASE
#define BRICKS_ENV_DEBUG 1
#endif

/* Features */
#if BRICKS_ENV_EXCEPTIONS
#define BRICKS_FEATURE_THROW(ext) throw ext
#define BRICKS_FEATURE_TRY try
#define BRICKS_FEATURE_CATCH(ext) catch(ext)
#define BRICKS_FEATURE_CATCH_EXCEPTION(ext, ex) catch(ext& ex)
#define BRICKS_FEATURE_CATCH_ALL catch(...)
#else
#define BRICKS_FEATURE_THROW(ext) abort()
#define BRICKS_FEATURE_TRY if(true)
#define BRICKS_FEATURE_CATCH(ex) while(false)
#define BRICKS_FEATURE_CATCH_EXCEPTION(ext, ex) for(ext ex;0;)
#define BRICKS_FEATURE_CATCH_ALL while(false)
#endif

#if BRICKS_ENV_RELEASE
#define BRICKS_FEATURE_RELEASE_THROW(ex) (void)0
#define BRICKS_FEATURE_RELEASE_THROW_FATAL(ex) abort()
#else
#define BRICKS_FEATURE_RELEASE_THROW(ex) BRICKS_FEATURE_THROW(ex)
#define BRICKS_FEATURE_RELEASE_THROW_FATAL(ex) BRICKS_FEATURE_THROW(ex)
#endif

#define BRICKS_ARRAY_COUNT(array) (sizeof(array) / sizeof(0[array]))

/* Attributes */
#if BRICKS_ENV_GCC
#define BRICKS_FEATURE_NORETURN __attribute__((noreturn))
#define BRICKS_FEATURE_CONSTRUCTOR(function) __attribute__((constructor))
#define BRICKS_FEATURE_DESTRUCTOR(function) __attribute__((destructor))
#define BRICKS_FEATURE_INLINE __attribute__((__always_inline__)) inline
#else
struct BRICKS_FEATURE_CONSTRUCTOR { BRICKS_FEATURE_CONSTRUCTOR(void (*function)()) { function(); } };
struct BRICKS_FEATURE_DESTRUCTOR { void (*function)(); BRICKS_FEATURE_DESTRUCTOR(void (*function)()) : function(function) { } ~BRICKS_FEATURE_DESTRUCTOR() { function(); } };
#define BRICKS_FEATURE_CONSTRUCTOR(function) void function(); static BRICKS_FEATURE_CONSTRUCTOR constructor_##function(function);
#define BRICKS_FEATURE_DESTRUCTOR(function) void function(); static BRICKS_FEATURE_DESTRUCTOR destructor_##function(function);
#if BRICKS_ENV_VCPP
#define BRICKS_FEATURE_NORETURN __declspec(noreturn)
#else
#define BRICKS_FEATURE_NORETURN
#endif
#endif

/* Logging */
#if BRICKS_CONFIG_LOGGING
#include <stdio.h>
#define BRICKS_FEATURE_LOG(...) do { fprintf(stderr, "[BRICKS] " __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr); } while (false)
#else
#define BRICKS_FEATURE_LOG(...)
#endif
#if BRICKS_CONFIG_LOGGING_HEAVY
#define BRICKS_FEATURE_LOG_HEAVY BRICKS_FEATURE_LOG
#else
#define BRICKS_FEATURE_LOG_HEAVY(...)
#endif

#if BRICKS_ENV_RELEASE
#define BRICKS_FEATURE_ASSERT(cond) ((void)0)
#else
#define BRICKS_FEATURE_ASSERT(cond) if (cond) (void)0; else BRICKS_FEATURE_THROW(InternalInconsistencyException("Assert failed: " # cond))
#endif

#if BRICKS_ENV_MINGW
#undef _GNU_SOURCE
#endif
