## Full install
```bash
git submodule init
git submodule update
```

## Build and test
```bash
mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j $(nproc) && clear && valgrind ./run_unit_tests --gtest_repeat=3
```