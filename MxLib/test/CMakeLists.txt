cmake_minimum_required(VERSION 3.10)

set (This MxLib_Test)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

set (Sources
    Tests.cpp
)

add_executable(${This} ${Sources})
target_link_libraries (${This} PUBLIC
    gtest_main
    MxLib
)

add_test (
    NAME ${This}
    COMMAND ${This}
)