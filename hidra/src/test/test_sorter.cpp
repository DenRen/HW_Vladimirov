#include "gtest/gtest.h"
#include "../bin/cppl.hpp"

#include <vector>
#include <algorithm>
#include <cmath>

using std::endl;
using std::cout;
using std::cerr;

template <typename T, typename U>
void half_sort (T* data, U size) {
    const U half_size = size / 2;

    if (half_size == 0) {
        return;
    }

    for (U i = 0; i < half_size; ++i) {
        if (data[i] > data[i + half_size])
            std::swap (data[i], data[i + half_size]);
    }

    half_sort (data, half_size);
    half_sort (data + half_size, half_size);
}

template <typename T>
void half_sort (std::vector <T>& vec) {
    half_sort (vec.data (), vec.size ());
}

template <typename T>
void print_vector (const std::vector <T>& vec) {
    cout << endl;
    for (const auto& val : vec)
        cout << val << " ";
    cout << endl;
}

template <typename T>
std::ostream& operator << (std::ostream& os, const std::vector <T>& vec) {
    std::size_t size = vec.size ();
    if (size == 0) {
        return os;
    }

    for (size_t i = 0; i < size - 1; ++i) {
        os << vec[i] << " ";
    }
    
    return os << vec[size - 1];
}

TEST (TEST_SORTER, TEST_HAL_FILTER) {
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefautDevice ();
    hidra::Sorter sorter (device);

    const size_t max_size_arr = 2 << 11; // Max 2 << 11
    const size_t repeat = 30;

    for (size_t i = 2; i < max_size_arr; i *= 2) {
        std::vector <int> data (i);
        for (size_t j = 0; j < repeat; ++j) {
            // Fill data
            for (size_t k = 0; k < i; ++k) {
                data[k] = 10 * sin (j + 10 * k) - k;
            }

            std::vector <int> half_sorted_data = data;
            half_sort (half_sorted_data);

            sorter.half_sort (data.data (), data.size ());

            ASSERT_TRUE (data == half_sorted_data) <<
                "data: " << data << endl << endl <<
                "res:  " << half_sorted_data << endl << endl;
        }
    }
}

TEST (TEST_SORTER, SIMPLE) {
    return;
    const size_t size = 1 << 2;     // TODO: if size == 1
    std::vector <int> A { 1, 5, 0, 7, 3, 4, 8, 1 };

    // for (int i = 0; i < size; ++i) {
    //     // A[i] = 10 * std::sin (i * 10);
    //     A[i] = -i * i + 3;
    // }
    
    std::cout << std::endl << std::endl;
    for (const auto& value : A) {
        std::cout << value << " ";
    }

    // --------------------------------------------------------
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefautDevice ();

    hidra::Sorter sorter (device);
    // --------------------------------------------------------

    auto ref_res = A;
    std::sort (ref_res.begin (), ref_res.end ());

    auto res = A;
    sorter.vect_sort (res.data (), res.size ());

    std::cout << std::endl << std::endl;
    for (const auto& value : res) {
        std::cout << value << " ";
    }
    std::cout << std::endl << std::endl;

    for (int i = 0; i < size; ++i) {
        ASSERT_EQ (ref_res[i], res[i]);
    }

}