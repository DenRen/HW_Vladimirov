#!/bin/bash

mkdir build &> /dev/null
cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
make -j $(nproc)
clear

./test_speed