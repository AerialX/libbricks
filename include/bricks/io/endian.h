#pragma once

#ifndef BRICKS_HEADER_BRICKS
#error Use bricks.h
#endif

#ifdef __GLIBC__
#include <endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define BRICKS_FEATURE_ENDIAN_LITTLE
#elif __BYTE_ORDER == __BIG_ENDIAN
#define BRICKS_FEATURE_ENDIAN_BIG
#endif
#elif defined(_BIG_ENDIAN)
#define BRICKS_FEATURE_ENDIAN_BIG
#elif defined(_LITTLE_ENDIAN)
#define BRICKS_FEATURE_ENDIAN_LITTLE
#endif

namespace Bricks { namespace IO {
	namespace Endian { enum Enum {
		Unknown = 0,
		Native,
		BigEndian,
		LittleEndian,
	}; }

#ifdef BRICKS_FEATURE_VCPP
#define BRICKS_ENDIAN_SWAP16(num) \
	_byteswap_ushort(num)
#define BRICKS_ENDIAN_SWAP32(num) \
	_byteswap_ulong(num)
#define BRICKS_ENDIAN_SWAP64(num) \
	_byteswap_uint64(num)
#else
#define BRICKS_ENDIAN_SWAP16(num) \
	((num << 8) | (num >> 8))
#ifdef BRICKS_FEATURE_GCC_4_3
#define BRICKS_ENDIAN_SWAP32(num) \
	__builtin_bswap32(num)
#define BRICKS_ENDIAN_SWAP64(num) \
	__builtin_bswap64(num)
#else
#define BRICKS_ENDIAN_SWAP32(num) \
	((num >> 24) | ((num & 0x00ff0000) >> 8) | ((num & 0x0000ff00) << 8) | (num << 24))
#define BRICKS_ENDIAN_SWAP64(num) \
	((BRICKS_ENDIAN_SWAP32((num & 0xffffffff)) << 32) | BRICKS_ENDIAN_SWAP32(num >> 32))
#endif
#endif

	static inline bool IsLittleEndian()
	{
#ifdef BRICKS_FEATURE_ENDIAN_LITTLE
		return true;
#elif defined(BRICKS_FEATURE_ENDIAN_BIG)
		return false;
#else
		union { u16 num; char buf[sizeof(u16)]; } endianness;
		endianness.num = 1;
		return endianness.buf[0];
#endif
	}
	static inline bool IsBigEndian() { return !IsLittleEndian(); }

#define BRICKS_ENDIAN_CONVERT(bits) \
	inline u##bits EndianConvertBE##bits(const void* data) { \
		const u##bits* const src = reinterpret_cast<const u##bits* const>(data); \
		if (IsBigEndian()) \
			return *src; \
		return BRICKS_ENDIAN_SWAP##bits(*src); \
	} \
	inline u##bits EndianConvertLE##bits(const void* data) { \
		const u##bits* const src = reinterpret_cast<const u##bits* const>(data); \
		if (IsLittleEndian()) \
			return *src; \
		return BRICKS_ENDIAN_SWAP##bits(*src); \
	} \
	template<typename T> \
	inline T EndianConvert##bits(Endian::Enum endianness, const void* data) { \
		if (endianness == Endian::BigEndian || (endianness == Endian::Native && IsBigEndian())) \
			return EndianConvertBE##bits(data); \
		else if (endianness == Endian::LittleEndian || (endianness == Endian::Native && IsLittleEndian())) \
			return EndianConvertLE##bits(data); \
		throw InvalidArgumentException("endianness"); \
	} \
	inline void EndianConvertBE##bits(void* dest, u##bits value) { \
		*reinterpret_cast<u##bits*>(dest) = IsBigEndian() ? value : BRICKS_ENDIAN_SWAP##bits(value); \
	} \
	inline void EndianConvertLE##bits(void* dest, u##bits value) { \
		*reinterpret_cast<u##bits*>(dest) = IsLittleEndian() ? value : BRICKS_ENDIAN_SWAP##bits(value); \
	} \
	inline void EndianConvert##bits(Endian::Enum endianness, void* data, u##bits value) { \
		if (endianness == Endian::BigEndian || (endianness == Endian::Native && IsBigEndian())) \
			return EndianConvertBE##bits(data, value); \
		else if (endianness == Endian::LittleEndian || (endianness == Endian::Native && IsLittleEndian())) \
			return EndianConvertLE##bits(data, value); \
		throw InvalidArgumentException("endianness"); \
	}

BRICKS_ENDIAN_CONVERT(16)
BRICKS_ENDIAN_CONVERT(32)
BRICKS_ENDIAN_CONVERT(64)

#undef BRICKS_ENDIAN_CONVERT
#undef BRICKS_ENDIAN_SWAP16
#undef BRICKS_ENDIAN_SWAP32
#undef BRICKS_ENDIAN_SWAP64

} }
