// #define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <stdio.h>
#include "mycllib.h"
#include "cppl.hpp"
#include <CL/cl2.hpp>
#include <vector>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

template <typename T>
void test_vector_add_in_arg (const size_t size = 1280000);

template <typename T>
void test_vector_add        (const size_t size = 1280000);

int
main (int argc, char* argv[]) {
    test_vector_add_in_arg <int> ();
    test_vector_add_in_arg <unsigned> ();
    test_vector_add_in_arg <long> ();
    test_vector_add_in_arg <unsigned long> ();
    
    test_vector_add <int> ();
    test_vector_add <unsigned> ();
    test_vector_add <long> ();
    test_vector_add <unsigned long> ();
}

template <typename T>
void test_vector_add_in_arg (const size_t size) {
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

    cl::Device device = device_provider.getDefautDevice ();
    hidra::Adder adder (device);
    
    adder.vect_add (A.data (), B.data (), B.data (), size);
    // --------------------------------------------------------

    for (size_t i = 0; i < size; ++i) {
        if (B[i] != ref_res[i]) {
            cerr << "Test failed!" << endl;
            cerr << "Index: i is " << i << endl
                 << "B[i] = " << B[i] << ", ref_res[i] = " << ref_res[i] << endl;
            return;
        }
    }

    cout << "Test passsed!" << endl;
}

template <typename T>
void test_vector_add (const size_t size) {
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

    cl::Device device = device_provider.getDefautDevice ();
    hidra::Adder adder (device);
    
    adder.vect_add (A.data (), B.data (), C.data (), size);
    // --------------------------------------------------------

    for (size_t i = 0; i < size; ++i) {
        if (C[i] != ref_res[i]) {
            cerr << "Test failed!" << endl;
            cerr << "Index: i is " << i << endl
                 << "C[i] = " << C[i] << ", ref_res[i] = " << ref_res[i] << endl;
            return;
        }
    }

    cout << "Test passsed!" << endl;
}

void c_style_main () {
    if (print_all_platforms_with_all_devices () == -1) {
        printf ("Error!\n");
    }

    learn_buf ();
    test_summ_array ();
}