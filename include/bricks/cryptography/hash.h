#pragma once

#include "bricks/object.h"
#include "bricks/io/stream.h"
#include "bricks/cryptography/hashalgorithm.h"

namespace Bricks { namespace Cryptography {
	class Hash : public Object
	{
	protected:
		AutoPointer<HashAlgorithm> algorithm;

	public:
		Hash(const Pointer<HashAlgorithm>& algorithm) : algorithm(algorithm) { }

		Data ComputeHash(const Data& data);
		Data ComputeHash(const Pointer<IO::Stream>& stream);

		// TODO: Interface to allow user to hash in blocks/chunks
	};
} }
