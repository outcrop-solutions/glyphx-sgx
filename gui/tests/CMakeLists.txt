cmake_minimum_required(VERSION 3.14)

# Set the project name
project(GlyphxUnitTests)

# Locate Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories("../glyphx")

# Find all test source files in the tests directory
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"  "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Add the production library or object files to the project
add_library(glyphx_lib STATIC ../glyphx/testTest.cpp)

# Create the test executable target
add_executable(unit_tests ${TEST_SOURCES}) 
target_link_libraries(unit_tests glyphx_lib gtest_main)

# Add a test to run the test executable
add_test(NAME unit_tests COMMAND unit_tests)
