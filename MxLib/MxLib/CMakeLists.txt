cmake_minimum_required(VERSION 3.0.0)

set (This MxLib)

project(${This} VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall")

enable_testing ()

set (Sources
    MxLib.cpp NumberMatrix.cpp
)

set (Headers
    MxLib.hpp NumberMatrix.hpp
)

add_library(${This} STATIC ${Sources})

#set (TEST test_${MXLIB})
#add_executable(${TEST} main_tests.cpp ${MXLIB_SRC})
#enable_testing()
#add_test (${TEST} ${TEST})

#target_link_libraries(${TEST} gtest pthread)