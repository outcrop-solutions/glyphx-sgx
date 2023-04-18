#include <gtest/gtest.h>
#include "testTest.h"
#define GTEST_HAS_TR1_TUPLE 0
TEST(TestTest, add) {
	Glyphx::Testing::TestTest testClass;
	auto result = testClass.add(2, 2);
	EXPECT_EQ(testClass.add(2, 2), 4);
	 
}

TEST(TestTest, subtract) {
	Glyphx::Testing::TestTest testClass;
	EXPECT_EQ(testClass.subtract(4, 2), 2);
}

