// NOTE: This is a special precompiled header libbricks builds for itself.
//       The sample uses it to speed up the build process, normal libbricks users should use their own.

#include <bricksall.hpp>

using namespace Bricks;
using namespace Bricks::IO;
using namespace Bricks::Collections;

static void testFilesystem()
{
	Console::Default->Out->WriteLine(" ==== Filesystem Tests ==== ");

	try {
		Console::Default->Out->WriteLine("Trying to open nonexistent file...");
		FileStream stream("lolnonexistent", FileOpenMode::Open, FileMode::ReadOnly);
	} catch (const Exception& ex) {
		Console::Default->Out->WriteLine(String::Format("Ohnoes! We got a %s! %s", ex.GetDebugString().CString(), ex.GetMessage().CString()));
	}

	Console::Default->Out->WriteLine("Writing test file...");

	Pointer<FileStream> stream = new FileStream("/tmp/libbricks-test.bin", FileOpenMode::Create, FileMode::WriteOnly, FilePermissions::OwnerReadWrite);

	Pointer<StreamWriter> writer = new StreamWriter(stream, Endian::BigEndian);
	writer->WriteInt32(0x1337BAAD);
	writer->WriteString("ohai");
	writer->WriteByte('\0');
	writer->WriteInt16(0xF33D, Endian::LittleEndian);

	// Manually delete these objects from memory.
	writer->Release();
	stream->Release();

	Console::Default->Out->WriteLine("Reading test file...");

	AutoPointer<FileStream> rstream = autonew FileStream("/tmp/libbricks-test.bin", FileOpenMode::Open, FileMode::ReadOnly);
	AutoPointer<StreamReader> reader = autonew StreamReader(rstream, Endian::LittleEndian);
	u32 num = reader->ReadInt32(Endian::BigEndian);
	BRICKS_FEATURE_ASSERT(num == 0x1337BAAD);
	String str = reader->ReadString();
	BRICKS_FEATURE_ASSERT(str == "ohai");
	u16 num2 = reader->ReadInt16();
	BRICKS_FEATURE_ASSERT(num2 == 0xF33D);
	//BRICKS_FEATURE_ASSERT(reader->IsEndOfFile()); // Not implemented yet, fail.
	rstream.Release();

	Console::Default->Out->WriteLine(String::Format("Success! Read back 0x%08x, \"%s\", and 0x%04x from file.", num, str.CString(), num2));

	// rstream and reader are both Released() when this returns thanks to AutoPointer.

	Console::Default->Out->WriteLine(" --- File Path Test --- ");
	Console::Default->Out->WriteLine("Creating FilePath, should be /test/lol/sup");
	FilePath path("lol");
	path = path.Combine("sup");
	path = path.RootPath("/test");
#ifndef BRICKS_FEATURE_WINDOWS
	BRICKS_FEATURE_ASSERT(!path.Compare("/test/lol/sup"));
#endif
	Console::Default->Out->WriteLine(String::Format("Result: %s, filename is %s", path.CString(), path.GetFileName().CString()));
	Console::Default->Out->WriteLine(String::Format("Directory is: %s", path.GetDirectory().CString()));

	Console::Default->Out->WriteLine(" --- Directory Iteration Test --- ");
	Console::Default->Out->WriteLine("Listing contents of current directory...");
	AutoPointer<FileNode> node = autonew FilesystemNode("."); // current dir
#ifdef BRICKS_CONFIG_CPP0X
	node->Iterate([](FileNode& subnode) -> bool {
		Console::Default->Out->WriteLine(String::Format("Subfile: %s", subnode.GetName().CString()));
		return true; // returning false is like break;ing out of the loop.
	});
#else
	foreach (FileNode& subnode, node) {
		Console::Default->Out->WriteLine(String::Format("Subfile: %s", subnode.GetName().CString()));
	}
#endif
}

static void testCollections()
{
	Console::Default->Out->WriteLine(" ==== Collection Tests ==== ");
	Console::Default->Out->WriteLine(" --- 3 Item Array Test --- ");
	AutoPointer<Array<int> > array = autonew Array<int>();
	array->AddItem(5);
	array->AddItem(3);
	array->AddItem(7);
	foreach (int item, array)
		Console::Default->Out->WriteLine(String::Format("Array Item: %d", item));

	Console::Default->Out->WriteLine(" --- <int, String> Dictionary Test --- ");
	Pointer<Dictionary<int, String> > dict = new Dictionary<int, String>();
	dict->Add(1, "one");
	dict->Add(2, "two");
	dict->Add(3, "three");
	Pair<int, String> item; // Must be declared outside of the foreach due to a preprocessor limitation
	foreach (item, dict)
		Console::Default->Out->WriteLine(String::Format("%d - %s", item.GetKey(), item.GetValue().CString()));
	Console::Default->Out->WriteLine(String::Format("Item at key 1: %s", dict->GetItem(1).CString()));
	dict->Release();
}

static void testObjects()
{
	Console::Default->Out->WriteLine(" ==== Object Tests ==== ");

	// Here we are creating an object, then retaining it and never letting go.
	// This is a memory leak.
	Console::Default->Out->WriteLine(" --- Memory Leak Test --- ");
	Console::Default->Out->WriteLine("#define BRICKS_CONFIG_LOGGING and BRICKS_CONFIG_LOGGING_MEMLEAK in config.h to see the leak report.");
	Pointer<Object> object = new Object();
	object->Retain();
	object->Release();
}

static void testDelegate()
{
	Console::Default->Out->WriteLine("ohai, call from static test delegate.");
}

class TestDelegateClass
{
protected:
	int value;

public:
	TestDelegateClass() : value(0) { }

	void testDelegateFunction()
	{
		Console::Default->Out->WriteLine(String::Format("ohai, call #%d from test delegate class", ++value));
	}
};

static void testDelegates()
{
	Console::Default->Out->WriteLine(" ==== Delegate Tests ==== ");

	Console::Default->Out->WriteLine(" --- Events Test --- ");
	Pointer<Delegate<void()> > delegate = new Delegate<void()>(testDelegate);
	Pointer<Event<void()> > event = new Event<void()>();
	TestDelegateClass test;
	*event += delegate;
	*event += delegate;
	*event += testDelegate;
	*event += Delegate<void()>(&test, &TestDelegateClass::testDelegateFunction);
	*event += Delegate<void()>(&test, &TestDelegateClass::testDelegateFunction);
	*event += MethodDelegate(test, &TestDelegateClass::testDelegateFunction);
	(*event)(); // Should print the first message thrice, and the second.

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
	Console::Default->Out->WriteLine("libbricks v" BRICKS_VERSION " test suite");
	Console::Default->Out->WriteLine("\tWhen you see it, you'll shit bricks.");
	Console::Default->Out->WriteLine();

	testFilesystem();
	Console::Default->Out->WriteLine();

	testCollections();
	Console::Default->Out->WriteLine();

	testObjects();
	Console::Default->Out->WriteLine();

	testDelegates();
	Console::Default->Out->WriteLine();

	return 0;
}
