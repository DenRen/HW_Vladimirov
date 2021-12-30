cmake_minimum_required (VERSION 3.0.0)
project (main VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 20)

# Main project
file (GLOB SOURCES "src/bin/*.cpp")

# Compiler flags
if (MSVC)
    set (CMAKE_CXX_FLAGS "/Wall")
    set (CMAKE_CXX_FLAGS "/EHsc") # For SFML compiling
    set (CMAKE_CXX_FLAGS_RELEASE_INIT "/O2y")
    set (CMAKE_CXX_FLAGS_DEBUG_INIT "/Odi")
else ()
    set (CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")
    set (CMAKE_CXX_FLAGS "-Wno-unused-parameter -Wno-missing-field-initializers") # For OpenCL include
    set (CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -flto")
    set (CMAKE_CXX_FLAGS_DEBUG_INIT "-O0 -g")
endif ()

#######################
# OpenCL
#######################
find_package (OpenCL REQUIRED)
include_directories (${OpenCL_INCLUDE_DIRS})
link_directories (${OpenCL_LIBRARY})

INCLUDE_DIRECTORIES ("OpenCL-CLHPP\\include")
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})
LINK_LIBRARIES (${OpenCL_LIBRARY})

#######################
# Main
#######################
set (SRC_DIR "src")
add_executable (main ${SOURCES} ${SRC_DIR}/main.cpp)

target_include_directories (main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (main ${OpenCL_LIBRARY})

add_custom_command (TARGET main POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/bin/config $<TARGET_FILE_DIR:main>)

#######################
# Test speed
#######################
set (TEST_DIR "src/test")

add_executable (test_speed ${SOURCES} ${TEST_DIR}/test_speed.cpp ${SRC_DIR}/main_test_speed.cpp)

target_include_directories (test_speed PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (test_speed ${OpenCL_LIBRARY})

add_custom_command (TARGET test_speed POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/bin/config $<TARGET_FILE_DIR:test_speed>)

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
add_executable (run_tests ${SOURCES}
                          ${TEST_DIR}/test_kernel_func.cpp ${TEST_DIR}/test_sorter.cpp)

target_include_directories (run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (run_tests ${OpenCL_LIBRARY})

add_custom_command (TARGET run_tests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/bin/config $<TARGET_FILE_DIR:run_tests>)

#Link test executable against gtest and gtest_main
target_link_libraries (run_tests gtest gtest_main ${OpenCL_LIBRARY})

