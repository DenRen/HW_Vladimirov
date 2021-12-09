#include "gtest/gtest.h"
#include "../bin/cppl.hpp"
#include "../bin/other_func.hpp"

#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

TEST (TEST_SORTER, TEST_VECTOR_SORT) {
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const size_t min_size_arr = 8 * 1 << 4;
    const size_t max_size_arr = 8 * 1 << 16;
    const size_t repeat = 10;

    for (std::size_t size_arr = min_size_arr; size_arr <= max_size_arr; size_arr *= 2) {
        for (std::size_t i = 0; i < repeat; ++i) {
            auto vec = getRandFillVector <int> (size_arr, mersenne);
            auto copy_vec = vec;

            sorter.vect_sort (vec.data (), vec.size ());

            checkEqual (copy_vec, vec);
        }
    }
} // TEST (TEST_SORTER, TEST_VECTOR_SORT)

TEST (TEST_SORTER, TEST_NEW_VECTOR_SORT) {
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const size_t min_size_arr = 8 * 1 << 4;
    const size_t max_size_arr = 8 * 1 << 16;
    const size_t repeat = 10;

    for (std::size_t size_arr = min_size_arr; size_arr <= max_size_arr; size_arr *= 2) {
        for (std::size_t i = 0; i < repeat; ++i) {
            auto vec = getRandFillVector <int> (size_arr, mersenne);
            auto copy_vec = vec;

            try {
                sorter.new_vect_sort (vec.data (), vec.size ());
            } catch (cl::Error& exc) {
                std::cout << exc.what () << ", error code: " << exc.err () << std::endl;
                throw;
            }

            checkEqual (copy_vec, vec);
        }
    }
}