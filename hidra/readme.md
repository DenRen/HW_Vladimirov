## Install OpenCL
sudo apt install opencl-headers

## Full install
```bash
git submodule init
git submodule update
```

## Build and test
```bash
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release ..
make -j $(nproc) && valgrind -q ./run_unit_tests --gtest_repeat=3
```