cmake_minimum_required (VERSION 3.0.0)
project (main VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto \
                        -lsfml-graphics -lsfml-window -lsfml-system")

add_executable (main main.cpp)
