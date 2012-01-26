#pragma once

#include "bricks/core/object.h"
#include "bricks/core/data.h"
#include "bricks/core/pointer.h"

namespace Bricks { namespace Cryptography {
	class HashAlgorithm : public Object
	{
	public:
		virtual Data Preprocess() { return Data(GetHashSize()); }
		virtual Data Process(const Data& hash, const Data& data) = 0;
		virtual Data Postprocess(const Data& hash) { return hash; }

		virtual size_t GetHashSize() const = 0;
	};

	template<typename M, int A, int S>
	class LinearCongruentialHashAlgorithm : public HashAlgorithm
	{
	public:
		size_t GetHashSize() const { return sizeof(M); }

		Data Preprocess() {
			M value = S;
			return Data(CastToRaw(&value), GetHashSize());
		}

		Data Process(const Data& hash, const Data& data) {
			M value = *CastToRaw<const M>(hash.GetData());
			for (size_t i = 0; i < data.GetLength(); i++)
				value = A * value + data[i];
			return Data(CastToRaw(&value), GetHashSize());
		}

		Data Postprocess(const Data& hash) {
			// TODO: Swap endianness.
			return hash;
		}
	};

	typedef LinearCongruentialHashAlgorithm<u32, 33, 5381> DJB2HashAlgorithm;
} }
