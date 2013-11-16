#include "bricks/cryptography/crc32.h"

namespace Bricks { namespace Cryptography {
	CRC32::CRC32()
	{
		for (int i = 0; i < 0x100; i++) {
			u32 n = i;
			for (int k = 0; k < 8; k++)
				n = (n >> 1) ^ ((n & 1) ? 0xedb88320L : 0);
			table[i] = n;
		}
	}

	Data CRC32::Preprocess()
	{
		u32 value = 0xffffffffUL;
		return Data(CastToRaw(&value), GetHashSize());
	}

	Data CRC32::Process(const Data& hash, const Data& data)
	{
		u32 value = *CastToRaw<const u32>(hash.GetData());
		for (size_t i = 0; i < data.GetLength(); i++)
			value = (value >> 8) ^ table[((s32)value ^ data[i]) & 0xff];
		return Data(CastToRaw(&value), GetHashSize());
	}

	Data CRC32::Postprocess(const Data& hash)
	{
		u32 value = *CastToRaw<const u32>(hash.GetData());
		value ^= 0xffffffffUL;
		return Data(CastToRaw(&value), GetHashSize());
	}
} }
