cmake_minimum_required (VERSION 3.0.0)
project (mivector VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

#######################
# GTest
#######################
ADD_SUBDIRECTORY (googletest)
enable_testing ()
include_directories (${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

#######################
# Unit Tests
#######################


# Add test cpp file
add_executable (run_unit_tests src/test_mivector.cpp)
#Link test executable against gtest and gtest_main
target_link_libraries (run_unit_tests gtest gtest_main)

add_executable (mivector src/main.cpp)


