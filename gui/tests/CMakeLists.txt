cmake_minimum_required(VERSION 3.10)
set(GTest_DIR "C:/Program Files (x86)/googletest-distribution/lib/cmake/GTest")
set(GTEST_INCLUDE_DIRS "C:/Program Files (x86)/googletest-distribution/include")
set(GTEST_LINK_DIRECTORY "C:/Program Files (x86)/googletest-distribution/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")

# Set the project name
project(GlyphxUnitTests)

# Locate Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories("../glyphx")

# Find all test source files in the tests directory
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")



# Add the production library or object files to the project
add_library(glyphx_lib STATIC ../glyphx/testTest.cpp)

# Link the test target to the production library and gtest_main
link_directories(${GTEST_LINK_DIRECTORY})

# Create the test executable target
add_executable(unit_tests ${TEST_SOURCES}) 
target_link_libraries(unit_tests glyphx_lib "${GTEST_LINK_DIRECTORY}/gtest_main.lib")


# Add a test to run the test executable
add_test(NAME unit_tests COMMAND unit_tests)