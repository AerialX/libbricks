#include <bricks.hpp>
#include <bricks/collections.hpp>
#include <bricks/io.hpp>

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Collections;

static void testFilesystem()
{
	Console::GetDefault()->Out->WriteLine(" ==== Filesystem Tests ==== ");

	BRICKS_FEATURE_TRY {
		Console::GetDefault()->Out->WriteLine("Trying to open nonexistent file...");
		FileStream stream("lolnonexistent", FileOpenMode::Open, FileMode::ReadOnly);
	} BRICKS_FEATURE_CATCH_EXCEPTION (Exception, ex) {
		Console::GetDefault()->Out->WriteLine(String::Format("Ohnoes! We got a %s! %s", ex.GetDebugString().CString(), ex.GetMessage().CString()));
	}

	Console::GetDefault()->Out->WriteLine("Writing test file...");

	FileStream* stream = new FileStream("/tmp/libbricks-test.bin", FileOpenMode::Create, FileMode::WriteOnly, FilePermissions::OwnerReadWrite);

	StreamWriter* writer = new StreamWriter(stream, Endian::BigEndian);
	writer->WriteInt32(0x1337BAAD);
	writer->WriteString("ohai");
	writer->WriteByte('\0');
	writer->WriteInt16(0xF33D, Endian::LittleEndian);

	// Manually delete these objects from memory.
	writer->Release();
	stream->Release();

	Console::GetDefault()->Out->WriteLine("Reading test file...");

	AutoPointer<FileStream> rstream = autonew FileStream("/tmp/libbricks-test.bin", FileOpenMode::Open, FileMode::ReadOnly);
	AutoPointer<StreamReader> reader = autonew StreamReader(rstream, Endian::LittleEndian);
	u32 num = reader->ReadInt32(Endian::BigEndian);
	BRICKS_FEATURE_ASSERT(num == 0x1337BAAD);
	String str = reader->ReadString();
	BRICKS_FEATURE_ASSERT(str == "ohai");
	u16 num2 = reader->ReadInt16();
	BRICKS_FEATURE_ASSERT(num2 == 0xF33D);
	BRICKS_FEATURE_ASSERT(reader->IsEndOfFile()); // Not implemented yet, fail.
	rstream.Release();

	Console::GetDefault()->Out->WriteLine(String::Format("Success! Read back 0x%08x, \"%s\", and 0x%04x from file.", num, str.CString(), num2));

	// rstream and reader are both Released() when this returns thanks to AutoPointer.

	Console::GetDefault()->Out->WriteLine(" --- File Path Test --- ");
	Console::GetDefault()->Out->WriteLine("Creating FilePath, should be /test/lol/sup");
	FilePath path("lol");
	path = path.Combine("sup");
	path = path.RootPath("/test");
#ifndef BRICKS_FEATURE_WINDOWS
	BRICKS_FEATURE_ASSERT(!path.Compare("/test/lol/sup"));
#endif
	Console::GetDefault()->Out->WriteLine(String::Format("Result: %s, filename is %s", path.CString(), path.GetFileName().CString()));
	Console::GetDefault()->Out->WriteLine(String::Format("Directory is: %s", path.GetDirectory().CString()));

	Console::GetDefault()->Out->WriteLine(" --- Directory Iteration Test --- ");
	Console::GetDefault()->Out->WriteLine("Listing contents of current directory...");
	AutoPointer<FileNode> node = autonew FilesystemNode("."); // current dir
#ifdef BRICKS_CONFIG_CPP0X
	node->Iterate([](FileNode& subnode) -> bool {
		Console::GetDefault()->Out->WriteLine(String::Format("Subfile: %s", subnode.GetName().CString()));
		return true; // returning false is like break;ing out of the loop.
	});
#else
	foreach (FileNode& subnode, node) {
		Console::GetDefault()->Out->WriteLine(String::Format("Subfile: %s", subnode.GetName().CString()));
	}
#endif
}

static void testCollections()
{
	Console::GetDefault()->Out->WriteLine(" ==== Collection Tests ==== ");
	Console::GetDefault()->Out->WriteLine(" --- 3 Item Array Test --- ");
	AutoPointer<Array<int> > array = autonew Array<int>();
	array->AddItem(5);
	array->AddItem(3);
	array->AddItem(7);
	foreach (int item, array)
		Console::GetDefault()->Out->WriteLine(String::Format("Array Item: %d", item));

	Console::GetDefault()->Out->WriteLine(" --- <int, String> Dictionary Test --- ");
	Dictionary<int, String>* dict = new Dictionary<int, String>();
	dict->Add(1, "one");
	dict->Add(2, "two");
	dict->Add(3, "three");
	Pair<int, String> item; // Must be declared outside of the foreach due to a preprocessor limitation
	foreach (item, dict)
		Console::GetDefault()->Out->WriteLine(String::Format("%d - %s", item.GetKey(), item.GetValue().CString()));
	Console::GetDefault()->Out->WriteLine(String::Format("Item at key 1: %s", dict->GetItem(1).CString()));
	dict->Release();
}

static void testObjects()
{
	Console::GetDefault()->Out->WriteLine(" ==== Object Tests ==== ");

	// Here we are creating an object, then retaining it and never letting go.
	// This is a memory leak.
	Console::GetDefault()->Out->WriteLine(" --- Memory Leak Test --- ");
	Console::GetDefault()->Out->WriteLine("#define BRICKS_CONFIG_LOGGING and BRICKS_CONFIG_LOGGING_MEMLEAK in config.h to see the leak report.");
	Object* object = new Object();
	object->Retain();
	object->Release();
}

static void testDelegate()
{
	Console::GetDefault()->Out->WriteLine("ohai, call from static test delegate.");
}

class TestDelegateClass
{
protected:
	int value;

public:
	TestDelegateClass() : value(0) { }

	void testDelegateFunction()
	{
		Console::GetDefault()->Out->WriteLine(String::Format("ohai, call #%d from test delegate class", ++value));
	}
};

static void testDelegates()
{
	Console::GetDefault()->Out->WriteLine(" ==== Delegate Tests ==== ");

	Console::GetDefault()->Out->WriteLine(" --- Events Test --- ");
	Delegate<void()>* delegate = new Delegate<void()>(testDelegate);
	Event<void()>* event = new Event<void()>();
	TestDelegateClass test;
	*event += delegate;
	*event += Delegate<void()>(testDelegate);
	*event += testDelegate;
	*event += Delegate<void()>(&test, &TestDelegateClass::testDelegateFunction);
	*event += MethodDelegate(test, &TestDelegateClass::testDelegateFunction);

	(*event)(); // Should print the first message thrice, and the second twice.

	// Remove the delegate (as many times as it's been added), and make sure the event is empty.
	*event -= delegate;
	*event -= test;
	BRICKS_FEATURE_ASSERT(!*event);
	(*event)();

	// Cleanup
	event->Release();
	delegate->Release();
}

int main(int argc, const char* argv[])
{
	// The console is really shitty.
	Console::GetDefault()->Out->WriteLine("libbricks v" BRICKS_VERSION " test suite");
	Console::GetDefault()->Out->WriteLine("\tWhen you see it, you'll shit bricks.");
	Console::GetDefault()->Out->WriteLine();

	testFilesystem();
	Console::GetDefault()->Out->WriteLine();

	testCollections();
	Console::GetDefault()->Out->WriteLine();

	testObjects();
	Console::GetDefault()->Out->WriteLine();

	testDelegates();
	Console::GetDefault()->Out->WriteLine();

	return 0;
}
