#include <assert.h>

#include <bricks.h>
#include <bricks/io.h>

using namespace Bricks;
using namespace Bricks::IO;

void testFilesystem()
{
	// Can't rely on a single ObjectPool in main or nothing will ever be released!
	ObjectPool pool;
	
	Console::Default.Out->WriteLine(" ==== Filesystem Tests ====");

	try {
		Console::Default.Out->WriteLine("Trying to open nonexistent file...");
		FileStream stream("lolnonexistent", FileOpenMode::Open, FileMode::ReadOnly);
		stream.Close();
	} catch (const Exception& ex) {
		Console::Default.Out->WriteLine(String::Format("Ohnoes! We got a %s! %s", ex.GetDebugString().CString(), ex.GetMessage().CString()));
	}

	Console::Default.Out->WriteLine("Writing test.bin...");

	// C++0x Alloc<>() syntax
	FileStream& stream = Alloc<FileStream>("test.bin", FileOpenMode::Create, FileMode::WriteOnly, FilePermissions::OwnerReadWrite);

	// Portable Alloc() syntax
	StreamWriter& writer = Alloc(StreamWriter, stream, Endian::BigEndian);
	writer.WriteInt32(0x1337BAAD);
	writer.WriteString("ohai");
	writer.WriteByte('\0');
	writer.WriteInt16(0xF33D, Endian::LittleEndian);
	stream.Close();

	// Makes these references useless, as they've been destroyed and cannot be reseated
	writer.Release();
	stream.Release();

	Console::Default.Out->WriteLine("Reading test.bin...");

	// Reseatable and nullable type
	Pointer<FileStream> rstream = AutoAlloc<FileStream>("test.bin", FileOpenMode::Open, FileMode::ReadOnly);
	Pointer<StreamReader> reader = AutoAlloc(StreamReader, rstream, Endian::LittleEndian);
	u32 num = reader->ReadInt32(Endian::BigEndian);
	assert(num == 0x1337BAAD);
	Pointer<String> str = reader->ReadString();
	u16 num2 = reader->ReadInt16();
	assert(num2 == 0xF33D);
	//assert(reader->IsEndOfFile()); // Not implemented yet, fail.
	rstream->Close();

	Console::Default.Out->WriteLine(String::Format("Success! Read back 0x%08x, \"%s\", and 0x%04x from file.", num, str->CString(), num2));

	// rstream and reader are both Released() when this returns thanks to the ObjectPool.
}

int main(int argc, const char* argv[])
{
	// Install a global ObjectPool for autorelease.
	ObjectPool pool;

	// The console is really shitty.
	Console::Default.Out->WriteLine("libbricks v" BRICKS_VERSION " test suite");
	Console::Default.Out->WriteLine("\tWhen you see it, you'll shit bricks.");

	testFilesystem();

	return 0;
}
