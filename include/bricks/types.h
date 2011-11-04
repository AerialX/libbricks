#pragma once

#include "bricks/config.h"

#ifndef __STDC_LIMIT_MACROS
	#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
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

/* Features */
#define BRICKS_FEATURE_MIN(a, b) ((a) < (b) ? (a) : (b))
#define BRICKS_FEATURE_MAX(a, b) ((a) > (b) ? (a) : (b))
#define BRICKS_FEATURE_ABS(a) ((a) < 0 ? -(a) : (a))
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
#ifdef __clang__
#define BRICKS_FEATURE_CLANG
#endif
#ifdef __OBJC__
#define BRICKS_FEATURE_OBJC
#endif
#ifdef __APPLE__
#define BRICKS_FEATURE_APPLE
#endif
#ifdef __MINGW32__
#define BRICKS_FEATURE_MINGW
#endif
#ifdef __linux__
#define BRICKS_FEATURE_LINUX
#endif
#ifdef __BSD__
#define BRICKS_FEATURE_BSD
#endif

#ifdef ANDROID
#define BRICKS_FEATURE_ANDROID
#endif

#if defined(BRICKS_FEATURE_LINUX) || defined(BRICKS_FEATURE_BSD)
#define BRICKS_FEATURE_LINUXBSD
#endif

#if defined(BRICKS_FEATURE_GCC) && !defined(BRICKS_FEATURE_MINGW)
#define BRICKS_FEATURE_UNIX
#endif

#if defined(BRICKS_FEATURE_MINGW) || defined(BRICKS_FEATURE_VCPP)
#define BRICKS_FEATURE_WINDOWS
#endif

#ifdef NDEBUG
#define BRICKS_FEATURE_RELEASE
#define BRICKS_FEATURE_RELEASE_THROW(ex) (void)0
#define BRICKS_FEATURE_RELEASE_THROW_FATAL(ex) abort()
#else
#define BRICKS_FEATURE_RELEASE_THROW(ex) throw ex
#define BRICKS_FEATURE_RELEASE_THROW_FATAL(ex) throw ex
#endif

/* Attributes */
#ifdef BRICKS_FEATURE_GCC
#define BRICKS_FEATURE_NORETURN __attribute__((noreturn))
#define BRICKS_FEATURE_CONSTRUCTOR(function) __attribute__((constructor))
#define BRICKS_FEATURE_DESTRUCTOR(function) __attribute__((destructor))
#else
struct BRICKS_FEATURE_CONSTRUCTOR { BRICKS_FEATURE_CONSTRUCTOR(void (*function)()) { function(); } };
struct BRICKS_FEATURE_DESTRUCTOR { void (*function)(); BRICKS_FEATURE_DESTRUCTOR(void (*function)()) : function(function) { } ~BRICKS_FEATURE_DESTRUCTOR() { function(); } };
#define BRICKS_FEATURE_CONSTRUCTOR(function) void function(); static BRICKS_FEATURE_CONSTRUCTOR constructor_##function(function);
#define BRICKS_FEATURE_DESTRUCTOR(function) void function(); static BRICKS_FEATURE_DESTRUCTOR destructor_##function(function);
#ifdef BRICKS_FEATURE_VCPP
#define BRICKS_FEATURE_NORETURN __declspec(noreturn)
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

#ifdef BRICKS_FEATURE_RELEASE
#define BRICKS_FEATURE_ASSERT(cond) ((void)0)
#else
#define BRICKS_FEATURE_ASSERT(cond) if (cond) (void)0; else throw InternalInconsistencyException("Assert failed: " # cond)
#endif

/* Missing Features */
#ifdef BRICKS_FEATURE_MINGW
#define S_IRWXG	00070
#define S_IRGRP	00040
#define S_IWGRP	00020
#define S_IXGRP	00100
#define S_IRWXO	00007
#define S_IROTH	00004
#define S_IWOTH	00002
#define S_IXOTH	00001
#define S_ISVTX	0001000
#define S_ISUID	0004000
#define S_ISGID	0002000

#define S_ISLNK(m)	0
#define S_ISSOCK(m)	0

#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12
#define DT_WHT		14

typedef short nlink_t;
typedef short uid_t;
typedef short gid_t;
typedef short blksize_t;
typedef short blkcnt_t;

#include <errno.h>
static inline int fsync(int fd) { errno = ENOSYS; return -1; }

#undef _GNU_SOURCE

#endif
