cmake_minimum_required (VERSION 3.0.0)
project (main VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")

add_compile_definitions (CL_HPP_ENABLE_EXCEPTIONS)

# Handle OpenCL
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})

#######################
# Main
#######################
set (BIN_DIR "src/bin")
add_executable (main ${BIN_DIR}/main.cpp
                     ${BIN_DIR}/cppl.cpp
                     ${BIN_DIR}/mycllib.cpp)

target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (main ${OpenCL_LIBRARY})

file (COPY src/bin/kernels DESTINATION .)

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
set (TEST_DIR "src/test")
add_executable (run_tests ${TEST_DIR}/test_sorter.cpp
                          ${TEST_DIR}/test_kernel_func.cpp
                          ${BIN_DIR}/cppl.cpp
                          ${BIN_DIR}/mycllib.cpp)
#Link test executable against gtest and gtest_main
target_link_libraries (run_tests gtest gtest_main ${OpenCL_LIBRARY})