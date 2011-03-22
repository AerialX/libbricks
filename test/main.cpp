#include <bricks.h>

#include <bricks/io.h>
#include <bricks/exception.h>

using namespace Bricks::IO;
using namespace Bricks;

#include <stdio.h>

int main(int argc, const char* argv[])
{
	try {
		FileStream stream("lol", FileCreateMode::Open, FileMode::ReadOnly);
		stream.Close();
	} catch (Exception ex) {
		printf("ohnoes! %s\n", ex.GetMessage().CString());
	}
	return 0;
}
