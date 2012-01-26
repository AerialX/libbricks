#include "brickstest.hpp"

#include <bricks/io/filestream.h>
#include <bricks/io/memorystream.h>
#include <bricks/io/cachestream.h>
#include <bricks/io/substream.h>

using namespace Bricks;
using namespace Bricks::IO;

TEST(BricksIoStreamTest, NonExistentFile) {
	EXPECT_THROW(FileStream("lolnonexistent", FileOpenMode::Open, FileMode::ReadOnly), ErrnoException) << "FileStream did not throw on non-existent file";
}

TEST(BricksIoStreamTest, EmptyFile) {
	FileStream stream(TestPath.Combine("data/empty.bin"), FileOpenMode::Open, FileMode::ReadOnly);
	u8 buffer[1];
	EXPECT_EQ(0, stream.Read(buffer, 1)) << "Stream read data from empty file";
}

static void WriteTest(Stream* stream)
{
	int value = 0xF88C;
	EXPECT_EQ(sizeof(value), stream->Write(&value, sizeof(value))) << "Stream write failed";
	value = 0x1888;
	EXPECT_EQ(sizeof(value), stream->Write(&value, sizeof(value))) << "Stream write failed";
}

static void ReadTest(Stream* stream)
{
	int value = 0;
	EXPECT_EQ(sizeof(value), stream->Read(&value, sizeof(value))) << "Stream read failed";
	EXPECT_EQ(0xF88C, value) << "Wrong stream data";
	EXPECT_EQ(sizeof(value), stream->Read(&value, sizeof(value))) << "Stream read failed";
	EXPECT_EQ(0x1888, value) << "Wrong stream data";
	EXPECT_EQ(sizeof(value) * 2, stream->GetPosition());
	EXPECT_EQ(stream->GetLength(), stream->GetPosition());
}

TEST(BricksIoStreamTest, WriteReadTest) {
	String path = "/tmp/libbricks-test.bin";
	{
		FileStream stream(path, FileOpenMode::Create, FileMode::ReadWrite, FilePermissions::OwnerReadWrite);
		WriteTest(&stream);
		stream.SetPosition(0);
		ReadTest(&stream);
	}

	{
		FileStream stream(path, FileOpenMode::Open, FileMode::ReadOnly, FilePermissions::OwnerReadWrite);
		ReadTest(&stream);
	}

	Filesystem::GetDefault()->DeleteFile(path);
}

TEST(BricksIoStreamTest, WriteReadSubstreamTest) {
	String path = "/tmp/libbricks-test.bin";
	{
		FileStream stream(path, FileOpenMode::Create, FileMode::WriteOnly, FilePermissions::OwnerReadWrite);
		WriteTest(&stream);
		WriteTest(&stream);
	}

	{
		FileStream stream(path, FileOpenMode::Open, FileMode::ReadOnly, FilePermissions::OwnerReadWrite);
		Substream substream(&stream, 0, stream.GetLength() / 2);
		ReadTest(&substream);
	}

	Filesystem::GetDefault()->DeleteFile(path);
}

TEST(BricksIoStreamTest, WriteReadCacheStreamTest) {
	String path = "/tmp/libbricks-test.bin";
	{
		FileStream stream(path, FileOpenMode::Create, FileMode::WriteOnly, FilePermissions::OwnerReadWrite);
		WriteTest(&stream);
	}

	{
		FileStream stream(path, FileOpenMode::Open, FileMode::ReadOnly, FilePermissions::OwnerReadWrite);
		CacheStream cachestream(&stream);
		ReadTest(&cachestream);
	}

	Filesystem::GetDefault()->DeleteFile(path);
}

TEST(BricksIoStreamTest, WriteReadMemoryStreamTest) {
	{
		MemoryStream stream;
		WriteTest(&stream);
		stream.SetPosition(0);
		ReadTest(&stream);
	}
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
