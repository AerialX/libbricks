#include "bricksall.hpp"

using namespace Bricks::IO;

namespace Bricks { namespace Cryptography {
	Data Hash::ComputeHash(const Data& data)
	{
		Data hash = algorithm->Preprocess();
		hash = algorithm->Process(hash, data);
		return algorithm->Postprocess(hash);
	}

	Data Hash::ComputeHash(const Pointer<Stream>& stream)
	{
		Data hash = algorithm->Preprocess();
		Data buffer(0x1000);
		size_t len;
		do {
			len = stream->Read(buffer);
			hash = algorithm->Process(hash, buffer);
		} while (len > 0);
		return algorithm->Postprocess(hash);
	}
} }
