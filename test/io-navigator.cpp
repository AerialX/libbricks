#include "brickstest.hpp"

TEST(BricksIoStreamTest, NonExistentFile) {
	EXPECT_THROW(FileStream("lolnonexistent", FileOpenMode::Open, FileMode::ReadOnly), ErrnoException) << "FileStream did not throw on non-existent file";
}

TEST(BricksIoNavigatorTest, EmptyFile) {
	FileStream stream(TestPath.Combine("data/empty.bin"), FileOpenMode::Open, FileMode::ReadOnly);
	StreamReader reader(stream);
	EXPECT_THROW(reader.ReadInt32(), EndOfStreamException) << "StreamReader did not throw on end of stream";
}

TEST(BricksIoNavigatorTest, WriteReadTest) {
	String path = "/tmp/libbricks-test.bin";
	const String testString = "ohai";
	const u32 testValue1 = 0x1337BAAD;
	const u16 testValue2 = 0xF33D;
	{ FileStream stream(path, FileOpenMode::Create, FileMode::WriteOnly, FilePermissions::OwnerReadWrite);
	StreamWriter writer(stream, Endian::BigEndian);
	writer.WriteInt32(testValue1);
	writer.WriteString(testString);
	writer.WriteByte('\0');
	writer.WriteInt16(testValue2, Endian::LittleEndian);
	EXPECT_EQ(sizeof(testValue1) + sizeof(testValue2) + testString.GetLength() + 1, stream.GetLength()) << "Written file size does not match"; }

	{ FileStream rstream(path, FileOpenMode::Open, FileMode::ReadOnly);
	StreamReader reader(rstream, Endian::LittleEndian);
	u32 num = reader.ReadInt32(Endian::BigEndian);
	EXPECT_EQ(testValue1, num);
	String str = reader.ReadString();
	EXPECT_EQ(testString, str);
	u16 num2 = reader.ReadInt16();
	EXPECT_EQ(testValue2, num2);
	EXPECT_TRUE(reader.IsEndOfFile()) << "We did not read the entire file"; }

	Filesystem::GetDefault()->DeleteFile(path);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
