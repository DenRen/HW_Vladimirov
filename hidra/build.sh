#!/bin/bash

mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
make -j $(nproc) && valgrind -q ./run_tests --gtest_repeat=1 && ./run_tests --gtest_repeat=3