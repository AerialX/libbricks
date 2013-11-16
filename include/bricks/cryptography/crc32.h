#pragma once

#include "bricks/cryptography/hashalgorithm.h"

namespace Bricks { namespace Cryptography {
	class CRC32 : public HashAlgorithm
	{
	protected:
		u32 table[0x100];

	public:
		CRC32();

		Data Preprocess();
		Data Process(const Data& hash, const Data& data);
		Data Postprocess(const Data& hash);

		size_t GetHashSize() const { return 4; }
	};
} }
