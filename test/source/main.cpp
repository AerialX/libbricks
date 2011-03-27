#include <assert.h>

#include <bricks.h>
#include <bricks/io.h>
#include <bricks/collections.h>
#include <bricks/collections/array.h>
#include <bricks/collections/dictionary.h>

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Collections;

void testFilesystem()
{
	// Can't rely on a single ObjectPool in main or nothing will ever be released!
	ObjectPool pool;
	
	Console::Default.Out->WriteLine(" ==== Filesystem Tests ==== ");

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

void testCollections()
{
	// Can't rely on a single ObjectPool in main or nothing will ever be released!
	ObjectPool pool;
	
	Console::Default.Out->WriteLine(" ==== Collection Tests ==== ");
	Console::Default.Out->WriteLine(" --- 3 Item Array Test --- ");
	Pointer<Array<int> > array = AutoAlloc(Array<int>);
	array->AddItem(5);
	array->AddItem(3);
	array->AddItem(7);
	foreach (int item, *array)
		Console::Default.Out->WriteLine(String::Format("Array Item: %d", item).CString());

	Console::Default.Out->WriteLine(" --- <int, String> Dictionary Test --- ");
	Pointer<Dictionary<int, String> > dict = Alloc<Dictionary<int, String> >();
	dict->Add(1, "one");
	dict->Add(2, "two");
	dict->Add(3, "three");
	foreach (auto item, *dict)
		Console::Default.Out->WriteLine(String::Format("%d - %s", item.GetKey(), item.GetValue().CString()).CString());
	Console::Default.Out->WriteLine(String::Format("Item at key 1: %s", dict->GetItem(1).CString()).CString());
	dict->Release();
}

int main(int argc, const char* argv[])
{
	// Install a global ObjectPool for autorelease.
	ObjectPool pool;

	// The console is really shitty.
	Console::Default.Out->WriteLine("libbricks v" BRICKS_VERSION " test suite");
	Console::Default.Out->WriteLine("\tWhen you see it, you'll shit bricks.");
	Console::Default.Out->WriteLine();

	testFilesystem();
	Console::Default.Out->WriteLine();

	testCollections();

	return 0;
}
