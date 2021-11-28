#include "gtest/gtest.h"
#include "../bin/cppl.hpp"

#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

using std::endl;
using std::cout;
using std::cerr;

template <typename T, typename U>
void
swap_if_greater (T* data, U pos_first, U pos_second) {
    if (data[pos_first] > data[pos_second]) {
        std::swap (data[pos_first], data[pos_second]);
    }
}

template <typename T, typename U>
void half_sort (T* data, U size) {
    const U half_size = size / 2;

    if (half_size == 0) {
        return;
    }

    for (U i = 0; i < half_size; ++i) {
        swap_if_greater (data, i, i + half_size);
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
    for (const auto& val : vec) {
        cout << val << " ";
    }
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

TEST (TEST_SORTER, TEST_HALF_FILTER) {
    return;
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne(rd());

    const size_t max_size_arr = 1 << 11; // Max 1 << 11
    const size_t repeat = 100;

    for (size_t i = 2; i <= max_size_arr; i *= 2) {
        std::vector <int> data (i);
        for (size_t j = 0; j < repeat; ++j) {
            // Fill data
            for (size_t k = 0; k < i; ++k) {
                data[k] = mersenne ();
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

template <typename T, typename U>
void unifying_network (T* data, U size) {
    for (U i = 0; i < size / 2; ++i) {
        swap_if_greater (data, i, size - i - 1);
    }

    half_sort (data, size / 2);
    half_sort (data + size / 2, size / 2);
}

template <typename T>
void unifying_network (std::vector <T>& vec) {
    unifying_network (vec.data (), vec.size ());
}

TEST (TEST_SORTER, TEST_UNIFUING_NETWORK) {
    return;
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne(rd());

    const size_t max_size_arr = 1 << 11; // Max 1 << 11
    const size_t repeat = 100;

    for (size_t i = 2; i <= max_size_arr; i *= 2) {
        std::vector <int> data (i);
        for (size_t j = 0; j < repeat; ++j) {
            // Fill data
            for (size_t k = 0; k < i; ++k) {
                data[k] = mersenne ();
            }

            std::vector <int> ref_data = data;
            unifying_network (ref_data);

            sorter.unifying_network (data.data (), data.size ());

            ASSERT_TRUE (data == ref_data) <<
                "ref: " << ref_data << endl << endl <<
                "res: " << data << endl << endl;
        }
    }
}

TEST (TEST_SORTER, TEST_VECTOR_SORT) {
    return;
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const size_t max_size_arr = 1 << 10; // Max 1 << 11
    const size_t repeat = 100;

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

TEST (TEST_SORTER, TEST_VECTOR_SORT_int1024) {
    return;
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const size_t size_arr = 2*8192;
    const size_t repeat = 1;

    std::vector <int> data (size_arr);
    for (size_t j = 0; j < repeat; ++j) {
        // Fill data
        for (size_t k = 0; k < data.size (); ++k) {
            data[k] = mersenne () % 30;
        }

        std::vector <int> ref_data = data;
        std::sort (ref_data.begin (), ref_data.end ());

        sorter.vect_sort (data.data (), data.size ());

        ASSERT_TRUE (data == ref_data) <<
            "ref: " << ref_data << endl << endl <<
            "res: " << data << endl << endl;
    }
}