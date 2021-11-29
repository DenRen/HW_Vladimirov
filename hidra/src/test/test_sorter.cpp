#include "gtest/gtest.h"
#include "../bin/cppl.hpp"
#include "../bin/other_func.hpp"

#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

using std::endl;
using std::cout;
using std::cerr;

TEST (TEST_SORTER, TEST_VECTOR_SORT) {
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const size_t max_size_arr = 1 << 13; // Max 1 << 11
    const size_t repeat = 10;

    for (std::size_t i = 0; i < repeat; ++i) {
        auto vec = getRandFillVector <int> (max_size_arr, mersenne, 50);
        auto copy_vec = vec;

        sorter.vect_sort (vec.data (), vec.size ());

        checkEqual (copy_vec, vec);
    }
}