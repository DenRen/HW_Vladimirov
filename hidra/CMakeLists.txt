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
# Sort
#######################
set (SRC_DIR "src")
add_executable (sort ${SOURCES} ${SRC_DIR}/sort.cpp)

target_include_directories (sort PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (sort ${OpenCL_LIBRARY})

add_custom_command (TARGET sort POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/bin/config $<TARGET_FILE_DIR:sort>)

#######################
# Test speed sorter
#######################
set (SORTER_TEST_DIR "src/test/sorter")

add_executable (sorter_test_speed ${SOURCES}
                                  ${SORTER_TEST_DIR}/test_speed.cpp
                                  ${SRC_DIR}/sorter_test_speed.cpp)

target_include_directories (sorter_test_speed PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (sorter_test_speed ${OpenCL_LIBRARY})

add_custom_command (TARGET sorter_test_speed POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/bin/config $<TARGET_FILE_DIR:sorter_test_speed>)

#######################
# GTest
#######################
ADD_SUBDIRECTORY (googletest)
enable_testing ()
include_directories ("googletest/googletest/include" ${gtest_SOURCE_DIR})

#######################
# Unit Tests sorter
#######################
# Add test cpp file
add_executable (sorter_run_tests ${SOURCES}
                                 ${SORTER_TEST_DIR}/test_kernel_func.cpp
                                 ${SORTER_TEST_DIR}/test_sorter.cpp)

target_include_directories (sorter_run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (sorter_run_tests ${OpenCL_LIBRARY})

add_custom_command (TARGET sorter_run_tests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/bin/config $<TARGET_FILE_DIR:sorter_run_tests>)

#Link test executable against gtest and gtest_main
target_link_libraries (sorter_run_tests gtest gtest_main ${OpenCL_LIBRARY})

#######################
# Find
#######################
set (SRC_DIR "src")
set (FINDER_TEST_DIR "src/test/finder")
add_executable (find ${SOURCES}
                     ${SRC_DIR}/find.cpp
                     ${SRC_DIR}/find_impl.cpp)

target_include_directories (find PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (find ${OpenCL_LIBRARY})

add_custom_command (TARGET find POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/bin/config $<TARGET_FILE_DIR:find>)


#######################
# Unit Tests finder
#######################
# Add test cpp file
add_executable (finder_run_tests ${SOURCES}
                                 ${FINDER_TEST_DIR}/test_finder.cpp)

target_include_directories (finder_run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (finder_run_tests ${OpenCL_LIBRARY})

add_custom_command (TARGET finder_run_tests POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/src/bin/config $<TARGET_FILE_DIR:finder_run_tests>)

#Link test executable against gtest and gtest_main
target_link_libraries (finder_run_tests gtest gtest_main ${OpenCL_LIBRARY})
