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
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    set (CMAKE_CXX_FLAGS "/Wall")
    set (CMAKE_CXX_FLAGS "/EHsc") # For SFML compiling
    set (CMAKE_CXX_FLAGS_RELEASE_INIT "/O2y")
    set (CMAKE_CXX_FLAGS_DEBUG_INIT "/Odi")
else ()
    set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
    set (CMAKE_CXX_FLAGS "-Wno-unused-parameter -Wno-missing-field-initializers") # For OpenCL include
    set (CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")
    set (CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g")
endif ()

# OpenCL
find_package (OpenCL REQUIRED)
include_directories (${OpenCL_INCLUDE_DIRS})
link_directories (${OpenCL_LIBRARY})

target_include_directories (clbrod PUBLIC "OpenCL-CLHPP\\include")
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

add_custom_command (TARGET clbrod POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/config $<TARGET_FILE_DIR:clbrod>)