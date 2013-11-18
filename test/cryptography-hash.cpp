#include "brickstest.hpp"

#include <bricks/core/value.h>
#include <bricks/cryptography/hash.h>
#include <bricks/cryptography/crc32.h>

using namespace Bricks;
using namespace Bricks::Cryptography;

TEST(BricksCollectionsCryptographyTest, CRC32) {
	CRC32 crc32;
	Hash hash(tempnew crc32);
	Value value(hash.ComputeHash(Data("hello")), ValueType::Int32);
	EXPECT_EQ(907060870, value.GetInt32Value());
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
