cmake_minimum_required (VERSION 3.0.0)
project (clbrod
    VERSION 0.1.0
    LANGUAGES CXX
)

# Main project
file (GLOB SOURCES "src/*.cpp")
add_executable (clbrod ${SOURCES})

set_target_properties (clbrod
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Werror"
)

# OpenCL
find_package (OpenCL REQUIRED)
include_directories (${OpenCL_INCLUDE_DIRS})
link_directories (${OpenCL_LIBRARY})

target_include_directories (clbrod PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (clbrod ${OpenCL_LIBRARY})

# SFML
set (BUILD_SHARED_LIBS FALSE)
add_subdirectory (SFML)

target_link_libraries (clbrod
    sfml-graphics
    sfml-window
    sfml-system
)

file (COPY src/kernels DESTINATION .)
file (COPY src/fonts DESTINATION .)