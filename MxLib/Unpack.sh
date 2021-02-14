#!/bin/bash

mkdir build
cd build
cmake ..
make -j 4
./test/MxLib_Test