#include "gtest/gtest.h"
#include "../bin/cppl.hpp"

#include <vector>
#include <algorithm>
#include <cmath>

TEST (TEST_SORTER, SIMPLE) {
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