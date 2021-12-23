#!/bin/bash

mkdir build &> /dev/null
cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

valgrind -q ./run_tests --gtest_repeat=1 && ./run_tests --gtest_repeat=3