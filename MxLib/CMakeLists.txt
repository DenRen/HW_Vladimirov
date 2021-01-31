cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 1.0)

enable_testing()

add_subdirectory (MxLib MxLib/Build)
add_subdirectory (googletest)
add_subdirectory (test)