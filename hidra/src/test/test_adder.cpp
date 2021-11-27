#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <stdio.h>

#include "../bin/mycllib.h"
#include "../bin/cppl.hpp"

using std::endl;

// #define TEST_ALL

template <typename T>
void test_vector_add_in_arg (const size_t size = 1280000) {
    // Prepare args

    // Here we may set folow types: int, long, unsigned, unsigned long
    std::vector <T> A (size), B (size), ref_res (size);

    const size_t buf_size = size * sizeof (A[0]);

    for (size_t i = 0; i < size; ++i) {
        A[i] = 14 * i;
        B[i] = i * i + 34 * i;
        ref_res[i] = A[i] + B[i];
    }

    // --------------------------------------------------------
    hidra::DeviceProvider device_provider;

    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Adder adder (device);
    
    adder.vect_add (A.data (), B.data (), B.data (), size);
    // --------------------------------------------------------

    for (size_t i = 0; i < size; ++i) {
        ASSERT_EQ (B[i], ref_res[i])
            << "Test failed!" << endl
            << "Index: i is " << i << endl
            << "B[i] = " << B[i] << ", ref_res[i] = " << ref_res[i] << endl;
    }
}

TEST (TEST_ADDER, VECTOR_ADD_IN_ARG) {
#ifdef TEST_ALL
    test_vector_add_in_arg <int> ();
    test_vector_add_in_arg <unsigned> ();
    test_vector_add_in_arg <long> ();
    test_vector_add_in_arg <unsigned long> ();
#endif // TEST_ALL
}

template <typename T>
void test_vector_add (const size_t size = 1280000) {
    // Prepare args

    // Here we may set folow types: int, long, unsigned, unsigned long
    std::vector <T> A (size), B (size), C (size), ref_res (size);

    const size_t buf_size = size * sizeof (A[0]);

    for (size_t i = 0; i < size; ++i) {
        A[i] = 14 * i;
        B[i] = i * i + 34 * i;
        ref_res[i] = A[i] + B[i];
    }

    // --------------------------------------------------------
    hidra::DeviceProvider device_provider;

    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Adder adder (device);
    
    adder.vect_add (A.data (), B.data (), C.data (), size);
    // --------------------------------------------------------

    for (size_t i = 0; i < size; ++i) {
        ASSERT_EQ (C[i], ref_res[i])
            << "Test failed!" << endl
            << "Index: i is " << i << endl
            << "C[i] = " << C[i] << ", ref_res[i] = " << ref_res[i] << endl;
    }
}

TEST (TEST_ADDER, VECTOR_ADD) {
#ifdef TEST_ALL
    test_vector_add <int> ();
    test_vector_add <unsigned> ();
    test_vector_add <long> ();
    test_vector_add <unsigned long> ();
#endif
}