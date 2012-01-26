#pragma once

#include "bricks/core/object.h"
#include "bricks/core/data.h"
#include "bricks/core/autopointer.h"
#include "bricks/cryptography/hashalgorithm.h"

namespace Bricks { namespace IO { class Stream; } }

namespace Bricks { namespace Cryptography {
	class Hash : public Object
	{
	protected:
		AutoPointer<HashAlgorithm> algorithm;

	public:
		Hash(HashAlgorithm* algorithm) : algorithm(algorithm) { }

		Data ComputeHash(const Data& data);
		Data ComputeHash(IO::Stream* stream);

		// TODO: Interface to allow user to hash in blocks/chunks
	};
} }
