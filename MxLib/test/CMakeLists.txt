cmake_minimum_required(VERSION 3.10)

set (This MxLib_Test)

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