#include "brickstest.hpp"

#include <bricks/collections/autoarray.h>
#include <bricks/collections/listguard.h>

using namespace Bricks;
using namespace Bricks::Collections;

TEST(BricksCollectionsListGuardTest, Test) {
	ArrayGuard<int> guarded;
	int i;
	for (i = 0; i < 4; i++)
		guarded.AddItem(i);

	i = 0;
	int ocount = guarded.GetCount();
	foreach (int num, guarded) {
		EXPECT_EQ(i, num);
		i++;

		guarded.AddItem(num);
		ocount++;

		guarded.RemoveItemAt(i - 1);
		ocount--;

		ocount += ocount;
		foreach (int subnum, guarded)
			guarded.AddItem(subnum);
	}
	EXPECT_EQ(4, i);
	EXPECT_EQ(ocount, guarded.GetCount());
}

TEST(BricksCollectionsListGuardTest, ObjectTest) {
	Array<AutoPointer<String> > list;
	ListGuard<Array<AutoPointer<String> > > guarded(tempnew list);
	guarded.AddItem(autonew String("ohai"));
	guarded.AddItem(autonew String("thar"));
	guarded.AddItem(autonew String("sir "));

	foreach (String* string, guarded) {
		EXPECT_EQ(4, string->GetLength());
		guarded.RemoveItemAt(0);
	}
	EXPECT_EQ(0, guarded.GetCount());
}

TEST(BricksCollectionsListGuardTest, ObjectAutoArrayTest) {
	AutoArray<String> list;
	ListGuard<AutoArray<String> > guarded(tempnew list);
	guarded.AddItem(autonew String("ohai"));
	guarded.AddItem(autonew String("thar"));
	guarded.AddItem(autonew String("sir "));

	foreach (String* string, guarded) {
		EXPECT_EQ(4, string->GetLength());
		guarded.RemoveItemAt(0);
	}
	EXPECT_EQ(0, guarded.GetCount());
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
