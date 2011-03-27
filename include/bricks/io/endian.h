#pragma once

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

	inline u16 EndianConvertBE16(const void* data) {
		const u16* const src = reinterpret_cast<const u16* const>(data);
		if (IsBigEndian())
			return *src;
		return BRICKS_ENDIAN_SWAP16(*src);
	}

	inline u32 EndianConvertBE32(const void* data) {
		const u32* const src = reinterpret_cast<const u32* const>(data);
		if (IsBigEndian())
			return *src;
		return BRICKS_ENDIAN_SWAP32(*src);
	}

	inline u64 EndianConvertBE64(const void* data) {
		const u64* const src = reinterpret_cast<const u64* const>(data);
		if (IsBigEndian())
			return *src;
		return BRICKS_ENDIAN_SWAP64(*src);
	}

	inline u16 EndianConvertLE16(const void* data) {
		const u16* const src = reinterpret_cast<const u16* const>(data);
		if (IsLittleEndian())
			return *src;
		return BRICKS_ENDIAN_SWAP16(*src);
	}

	inline u32 EndianConvertLE32(const void* data) {
		const u32* const src = reinterpret_cast<const u32* const>(data);
		if (IsLittleEndian())
			return *src;
		return BRICKS_ENDIAN_SWAP32(*src);
	}

	inline u64 EndianConvertLE64(const void* data) {
		const u64* const src = reinterpret_cast<const u64* const>(data);
		if (IsLittleEndian())
			return *src;
		return BRICKS_ENDIAN_SWAP64(*src);
	}
	
	template<typename T>
	inline T EndianConvert16(Endian::Enum endianness, const void* data) {
		if (endianness == Endian::BigEndian || (endianness == Endian::Native && IsBigEndian()))
			return EndianConvertBE16(data);
		else if (endianness == Endian::LittleEndian || (endianness == Endian::Native && IsLittleEndian()))
			return EndianConvertLE16(data);
		Throw(InvalidArgumentException, "endianness");
	}
	template<typename T>
	inline T EndianConvert32(Endian::Enum endianness, const void* data) {
		if (endianness == Endian::BigEndian || (endianness == Endian::Native && IsBigEndian()))
			return EndianConvertBE32(data);
		else if (endianness == Endian::LittleEndian || (endianness == Endian::Native && IsLittleEndian()))
			return EndianConvertLE32(data);
		Throw(InvalidArgumentException, "endianness");
	}
	template<typename T>
	inline T EndianConvert64(Endian::Enum endianness, const void* data) {
		if (endianness == Endian::BigEndian || (endianness == Endian::Native && IsBigEndian()))
			return EndianConvertBE64(data);
		else if (endianness == Endian::LittleEndian || (endianness == Endian::Native && IsLittleEndian()))
			return EndianConvertLE64(data);
		Throw(InvalidArgumentException, "endianness");
	}

	inline void EndianConvertBE16(void* dest, u16 value) {
		*reinterpret_cast<u16*>(dest) = IsBigEndian() ? value : BRICKS_ENDIAN_SWAP16(value);
	}
	inline void EndianConvertBE32(void* dest, u32 value) {
		*reinterpret_cast<u32*>(dest) = IsBigEndian() ? value : BRICKS_ENDIAN_SWAP32(value);
	}
	inline void EndianConvertBE64(void* dest, u64 value) {
		*reinterpret_cast<u64*>(dest) = IsBigEndian() ? value : BRICKS_ENDIAN_SWAP64(value);
	}
	inline void EndianConvertLE16(void* dest, u16 value) {
		*reinterpret_cast<u16*>(dest) = IsLittleEndian() ? value : BRICKS_ENDIAN_SWAP16(value);
	}
	inline void EndianConvertLE32(void* dest, u32 value) {
		*reinterpret_cast<u32*>(dest) = IsLittleEndian() ? value : BRICKS_ENDIAN_SWAP32(value);
	}
	inline void EndianConvertLE64(void* dest, u64 value) {
		*reinterpret_cast<u64*>(dest) = IsLittleEndian() ? value : BRICKS_ENDIAN_SWAP64(value);
	}

	inline void EndianConvert16(Endian::Enum endianness, void* data, u16 value) {
		if (endianness == Endian::BigEndian || (endianness == Endian::Native && IsBigEndian()))
			return EndianConvertBE16(data, value);
		else if (endianness == Endian::LittleEndian || (endianness == Endian::Native && IsLittleEndian()))
			return EndianConvertLE16(data, value);
		Throw(InvalidArgumentException, "endianness");
	}
	inline void EndianConvert32(Endian::Enum endianness, void* data, u32 value) {
		if (endianness == Endian::BigEndian || (endianness == Endian::Native && IsBigEndian()))
			return EndianConvertBE32(data, value);
		else if (endianness == Endian::LittleEndian || (endianness == Endian::Native && IsLittleEndian()))
			return EndianConvertLE32(data, value);
		Throw(InvalidArgumentException, "endianness");
	}
	inline void EndianConvert64(Endian::Enum endianness, void* data, u64 value) {
		if (endianness == Endian::BigEndian || (endianness == Endian::Native && IsBigEndian()))
			return EndianConvertBE64(data, value);
		else if (endianness == Endian::LittleEndian || (endianness == Endian::Native && IsLittleEndian()))
			return EndianConvertLE64(data, value);
		Throw(InvalidArgumentException, "endianness");
	}
} }
