#include "brickstest.hpp"

#include <bricks/core/value.h>

using namespace Bricks;

TEST(BricksCoreValueTest, Basic) {
	Value value(0);
	EXPECT_EQ(0, value.GetPointerValue());
	EXPECT_EQ(0, value.GetByteValue());
	EXPECT_EQ(0, value.GetInt16Value());
	EXPECT_EQ(0, value.GetInt32Value());
	EXPECT_EQ(0.0f, value.GetFloat32Value());
	EXPECT_EQ(0.0, value.GetFloat64Value());
}

TEST(BricksCoreValueTest, Integer) {
	Value value(0x1337);
	EXPECT_EQ(0x37, value.GetByteValue());
	EXPECT_EQ(0x1337, value.GetInt16Value());
	EXPECT_EQ(0x1337, value.GetInt32Value());
	EXPECT_EQ((float)0x1337, value.GetFloat32Value());
}

TEST(BricksCoreValueTest, Float) {
	Value value(10.0f);
	EXPECT_EQ(10, value.GetByteValue());
	EXPECT_EQ(10, value.GetInt32Value());
	EXPECT_EQ(10.0f, value.GetFloat32Value());
}

TEST(BricksCoreValueTest, Pointer) {
	int number = 0;
	Value value(&number);
	EXPECT_EQ(&number, value.GetPointerValue());
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
