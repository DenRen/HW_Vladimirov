cmake_minimum_required (VERSION 3.0.0)
project (main VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 \
                      -lsfml-graphics -lsfml-window -lsfml-system \
                      -lOpenCL")
# Handle OpenCL
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})
link_directories(${OpenCL_LIBRARY})


file (GLOB SOURCES "src/*.cpp")

add_executable (main ${SOURCES})


file (COPY src/kernels DESTINATION .)