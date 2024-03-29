#!/bin/bash

mkdir build &> /dev/null
cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j

valgrind -q ./sorter_run_tests --gtest_repeat=1 && ./sorter_run_tests --gtest_repeat=3
