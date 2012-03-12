#include "brickstest.hpp"

#include <bricks/core/autopointer.h>
#include <bricks/compression/zipfilesystem.h>
#include <bricks/io/filestream.h>
#include <bricks/io/navigator.h>

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Compression;

/* Archive contents. The files with contents contain their filename (such as "bdir/file1") followed by a linebreak.
Archive:  test.zip
 Length   Method    Size  Cmpr    Date    Time   CRC-32   Name
--------  ------  ------- ---- ---------- ----- --------  ----
       0  Stored        0   0% 2011-08-28 13:19 00000000  adir/
       0  Stored        0   0% 2011-08-27 13:14 00000000  adir/dir/
       0  Stored        0   0% 2011-08-27 13:14 00000000  adir/file
       0  Stored        0   0% 2011-08-28 13:19 00000000  bdir/
      11  Stored       11   0% 2011-08-28 13:19 cf9884df  bdir/file1
      11  Stored       11   0% 2011-08-28 13:19 e4b5d71c  bdir/file2
       0  Stored        0   0% 2011-08-27 13:14 00000000  cdir/
      11  Stored       11   0% 2011-08-28 13:19 7d57b11d  adir/file2
--------          -------  ---                            -------
      33               33   0%                            8 files
*/

class BricksCompressionZipFilesystemTest : public testing::Test
{
protected:
	AutoPointer<Stream> fileStream;
	virtual void SetUp()
	{
		fileStream = autonew FileStream(TestPath.Combine("data/test.zip"), FileOpenMode::Open, FileMode::ReadOnly);
	}

	virtual void TearDown()
	{
	}
};

TEST_F(BricksCompressionZipFilesystemTest, InitialParse) {
	try {
		ZipFilesystem zip(fileStream);
	} catch (const Exception& ex) {
		ADD_FAILURE();
	}
}

TEST_F(BricksCompressionZipFilesystemTest, Iteration) {
	ZipFilesystem zip(fileStream);
	FilesystemNode root("adir/", tempnew zip);
	int subnodecount = 0;
	foreach (FileNode* node, root) {
		subnodecount++;
	}
	EXPECT_EQ(3, subnodecount);
}

TEST_F(BricksCompressionZipFilesystemTest, Stat) {
	ZipFilesystem zip(fileStream);
	EXPECT_EQ(0, FilesystemNode("adir/file", tempnew zip).GetSize());
	EXPECT_EQ(sizeof("adir/file2"), FilesystemNode("adir/file2", tempnew zip).GetSize());
	EXPECT_EQ(sizeof("bdir/file1"), FilesystemNode("adir/file2", tempnew zip).GetSize());
	EXPECT_EQ(sizeof("bdir/file2"), FilesystemNode("adir/file2", tempnew zip).GetSize());
}

TEST_F(BricksCompressionZipFilesystemTest, Read) {
	ZipFilesystem zip(fileStream);
	FilesystemNode node("adir/file2", tempnew zip);
	AutoPointer<Stream> stream = node.OpenStream();
	String str = StreamReader(stream).ReadString();
	EXPECT_EQ(String("adir/file2\n"), str);
}

TEST_F(BricksCompressionZipFilesystemTest, ReadChdir) {
	ZipFilesystem zip(fileStream);
	zip.ChangeCurrentDirectory("adir");
	FilesystemNode node("file2", tempnew zip);
	AutoPointer<Stream> stream = node.OpenStream();
	String str = StreamReader(stream).ReadString();
	EXPECT_EQ(String("adir/file2\n"), str);
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
