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

    const size_t max_size_arr = 155 + (1 << 10); // Max 1 << 11
    const size_t repeat = 10;

    for (std::size_t i = 0; i < repeat; ++i) {
        auto vec = getRandFillVector <int> (max_size_arr, mersenne, 50);
        auto copy_vec = vec;

        sorter.vect_sort (vec.data (), vec.size ());

        checkEqual (copy_vec, vec);
    }

    return;
    for (size_t i = max_size_arr; i <= max_size_arr; i *= 2) {
        std::vector <int> data (8 * i);
        for (size_t j = 0; j < repeat; ++j) {
            // Fill data
            for (size_t k = 0; k < data.size (); ++k) {
                data[k] = mersenne ();
            }

            std::vector <int> ref_data = data;
            std::sort (ref_data.begin (), ref_data.end ());

            sorter.vect_sort (data.data (), data.size ());

            ASSERT_TRUE (data == ref_data)/* <<
                "ref: " << ref_data << endl << endl <<
                "res: " << data << endl << endl*/;
        }
    }
}