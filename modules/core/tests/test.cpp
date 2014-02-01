#include "gtest/gtest.h"
#include <string>


namespace {
	TEST(FakeTest, FakeTest) {
		const std::string name = "FakeTest";
		EXPECT_EQ(name, name);
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	int ret = RUN_ALL_TESTS();
	system("Pause");
	return ret;
}

