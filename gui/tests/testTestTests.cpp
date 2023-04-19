#include "gtest_namespace_tests.h"
#include "testTest.h"
#include <stdio.h>

//This is a test fixture.  Test suites can use test fixtures to share setup and tear down logic
//they do not share the instance of the fixture a new one is spun up for each test suite.
class TestTestFixture : public ::testing::Test {
protected: 
	void SetUp() override {
		//perform our setup here
	}

	void TearDown() override {
		//perform our teardown here
	}
    //Interesting stuff our tests can interact with
	Glyphx::Testing::TestTest testClass;
};

//These are basic tests that do not use our fixtures.

//                                  The package to test (no equivilent since Mocha runs tests as part of the package) 
//              The class to test (describe)
//                        The function to test (context) 
//                        
NAMESPACED_TEST(TestTest, subtract,  Glyphx) {
	     //Since these macros get compiled down into actual objects, there are no concepts of (it) like there are in mocha.
		 // Each test should create cases to fully test the function defined in the second argument (context). 
		 // So we loose the fidelity of being able to just test that a function throws an exception, but we can still break our testing 
		 // down into managagble chunks.
		  
	    Glyphx::Testing::TestTest testClass;
		//Google test supports both ASSERT Which stops the 
	    //the test as soon as it fails 
	    //and EXPECT which displays a failure but continues to run
	    //In this case, all of these statements will be analyzed
	    
		EXPECT_EQ(testClass.subtract(4, 2), 2);
	
		EXPECT_EQ(testClass.subtract(5, 2), 2);
	
		EXPECT_EQ(testClass.subtract(5, 2), 1);
	
	}

NAMESPACED_TEST(TestTest, add, Glyphx) {
	Glyphx::Testing::TestTest testClass;
	auto result = testClass.add(2, 2);
	ASSERT_EQ(testClass.add(2, 2), 4);  //This will run
	ASSERT_EQ(testClass.add(5, 2), 4);  //This will run 
	ASSERT_EQ(testClass.add(5, 2), 4);  //This will not run, the test will stop

}

//Notice the different name here -- it's a C++ macro limitation
NAMESPACED_TEST_F(TestTestFixture, add, Glyphx) {
	//test class is provided by our TestFixture
	EXPECT_EQ(testClass.add(4, 2), 6);

	EXPECT_EQ(testClass.add(5, 2), 7);

	EXPECT_EQ(testClass.add(5, 2), 7);
}

//This test will gets its very own copy of the TextFixture
NAMESPACED_TEST_F(TestTestFixture, subtract, Glyphx) {

	EXPECT_EQ(testClass.subtract(4, 2), 2);

	EXPECT_EQ(testClass.subtract(5, 2), 3);

	EXPECT_EQ(testClass.subtract(5, 2), 3);

}

