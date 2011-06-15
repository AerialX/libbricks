#include <assert.h>

#include <bricks.h>
#include <bricks/io.h>
#include <bricks/io/console.h>
#include <bricks/collections.h>
#include <bricks/collections/array.h>
#include <bricks/collections/dictionary.h>
#include <bricks/delegate.h>

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Collections;

static void testFilesystem()
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

	FileStream& stream = alloc FileStream("test.bin", FileOpenMode::Create, FileMode::WriteOnly, FilePermissions::OwnerReadWrite);

	StreamWriter& writer = alloc StreamWriter(stream, Endian::BigEndian);
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
	Pointer<FileStream> rstream = autoalloc FileStream("test.bin", FileOpenMode::Open, FileMode::ReadOnly);
	Pointer<StreamReader> reader = autoalloc StreamReader(rstream, Endian::LittleEndian);
	u32 num = reader->ReadInt32(Endian::BigEndian);
	assert(num == 0x1337BAAD);
	Pointer<String> str = reader->ReadString();
	u16 num2 = reader->ReadInt16();
	assert(num2 == 0xF33D);
	//assert(reader->IsEndOfFile()); // Not implemented yet, fail.
	rstream->Close();

	Console::Default.Out->WriteLine(String::Format("Success! Read back 0x%08x, \"%s\", and 0x%04x from file.", num, str->CString(), num2));

	// rstream and reader are both Released() when this returns thanks to the ObjectPool.

	Console::Default.Out->WriteLine(" --- File Path Test --- ");
	Console::Default.Out->WriteLine("Creating FilePath, should be /test/lol/sup");
	Pointer<FilePath> path = autoalloc FilePath("lol");
	*path = path->Combine("sup");
	*path = path->RootPath("/test");
	assert(!path->Compare("/test/lol/sup"));
	Console::Default.Out->WriteLine(String::Format("Result: %s, filename is %s", path->CString(), path->GetFileName().CString()));
	Console::Default.Out->WriteLine(String::Format("Directory is: %s", path->GetDirectory().CString()));

	Console::Default.Out->WriteLine(" --- Directory Iteration Test --- ");
	Console::Default.Out->WriteLine("Listing contents of current directory...");
	Pointer<FileNode> node = autoalloc FilesystemNode("."); // current dir
#ifdef BRICKS_CONFIG_CPP0X
	// Technically this will work even without libbricks configured for C++0x because the lambda decays to a function pointer,
	// but only when using a compiler that supports C++0x. And only because the capture list is empty.
	node->Iterate([](FileNode& subnode) -> bool {
		Console::Default.Out->WriteLine(String::Format("Subfile: %s", subnode.GetName().CString()));
		return true; // returning false is like break;ing out of the loop.
	});
#else
	foreach (FileNode& subnode, *node) {
		Console::Default.Out->WriteLine(String::Format("Subfile: %s", subnode.GetName().CString()));
	}
#endif
}

static void testCollections()
{
	// Can't rely on a single ObjectPool in main or nothing will ever be released!
	ObjectPool pool;
	
	Console::Default.Out->WriteLine(" ==== Collection Tests ==== ");
	Console::Default.Out->WriteLine(" --- 3 Item Array Test --- ");
	Pointer<Array<int> > array = autoalloc Array<int>();
	array->AddItem(5);
	array->AddItem(3);
	array->AddItem(7);
	foreach (int item, *array)
		Console::Default.Out->WriteLine(String::Format("Array Item: %d", item).CString());

	Console::Default.Out->WriteLine(" --- <int, String> Dictionary Test --- ");
	Pointer<Dictionary<int, String> > dict = new Dictionary<int, String>();
	dict->Add(1, "one");
	dict->Add(2, "two");
	dict->Add(3, "three");
	Pair<int, String> item; // Must be declared outside of the foreach due to a preprocessor limitation
	foreach (item, *dict)
		Console::Default.Out->WriteLine(String::Format("%d - %s", item.GetKey(), item.GetValue().CString()).CString());
	Console::Default.Out->WriteLine(String::Format("Item at key 1: %s", dict->GetItem(1).CString()).CString());
	dict->Release();
}

static void testObjects()
{
	// Can't rely on a single ObjectPool in main or nothing will ever be released!
	ObjectPool pool;
	
	Console::Default.Out->WriteLine(" ==== Object Tests ==== ");

	// Here we are adding an object to the ObjectPool, but then retaining it and never letting go.
	// This is a memory leak.
	Console::Default.Out->WriteLine(" --- Memory Leak Test --- ");
	Console::Default.Out->WriteLine("#define BRICKS_CONFIG_LOGGING and BRICKS_CONFIG_LOGGING_MEMLEAK in config.h to see the leak report.");
	Object& object = autoalloc Object();
	object.Retain();
}

static void testDelegate()
{
	Console::Default.Out->WriteLine("ohai, call from test delegate.");
}

static void testDelegates()
{
	// Can't rely on a single ObjectPool in main or nothing will ever be released!
	ObjectPool pool;
	Console::Default.Out->WriteLine(" ==== Delegate Tests ==== ");

	Console::Default.Out->WriteLine(" --- Events Test --- ");
	Delegate<void()>& delegate = alloc Delegate<void()>(testDelegate);
	Event<void()>& event = alloc Event<void()>();
	event += delegate;
	event += delegate;
	event(); // Should print the message twice.

	// Remove the delegate (as many times as it's been added), and make sure the event is empty.
	event -= delegate;
	assert(!event);
	event();

	// Cleanup
	event.Release();
	delegate.Release();
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
	Console::Default.Out->WriteLine();

	testObjects();
	Console::Default.Out->WriteLine();

	testDelegates();
	Console::Default.Out->WriteLine();

	return 0;
}
