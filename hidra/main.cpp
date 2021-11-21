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

#define STRINGIFY(...) #__VA_ARGS__

int
main (int argc, char* argv[]) {
    std::vector <cl::Platform> platforms;
    cl::Platform::get (&platforms);

    if (platforms.size () == 0) {
        cerr << "No platforms" << endl;
        return -1;
    }
    cout << "Founded " << platforms.size () << " platforms" << endl;

    cl::Platform platform;
    for (auto& iter_platform : platforms) {
        std::string platform_version = iter_platform.getInfo <CL_PLATFORM_VERSION> ();
        if (platform_version.find ("OpenCL 3.") != decltype (platform_version)::npos) {
            platform = iter_platform;
            break;
        }
    }

    if (platform () == NULL) {
        cerr << "No platforms with OpenCL 3" << endl;
        return -1;
    }
    cout << "Selected " << platform.getInfo <CL_PLATFORM_NAME> () << " platform" << endl;

    std::vector <cl::Device> devices;
    platform.getDevices (CL_DEVICE_TYPE_GPU, &devices);


    for (auto& i : devices) {
        cout << "Device: " << i.getInfo <CL_DEVICE_NAME> () << endl;
    }

    cl::Device device = devices[0];

    cl::Context context (device);
    cl::CommandQueue queue (context);

    const size_t size = 128;
    std::vector <int> A (size), B (size);

    const size_t buf_size = size * sizeof (A[0]);

    for (size_t i = 0; i < size; ++i) {
        A[i] = i;
        B[i] = i;
    }
    
    cl::Buffer buf_a (context, CL_MEM_READ_ONLY, buf_size);
    cl::Buffer buf_b (context, CL_MEM_READ_WRITE, buf_size);

    cl::copy (queue, A.data (), A.data () + size, buf_a);
    cl::copy (queue, B.data (), B.data () + size, buf_b);

    std::string source_vect_add (STRINGIFY (
        __kernel void vector_add (__global __read_only int* A, __global __read_write int* B) {
            int i = get_global_id (0);
            B[i] = A[i] + B[i];
        }
    ));

    cl::Program vect_add_program (context, source_vect_add, true);
    cl::KernelFunctor <cl::Buffer, cl::Buffer> add_vects (vect_add_program, "vector_add");

    cl::NDRange global (size);
    cl::NDRange local (64);
    cl::EnqueueArgs enc_args {queue, global, local};

    add_vects (enc_args, buf_a, buf_b);

    cl::copy (queue, buf_b, B.data (), B.data () + size);

    for (size_t i = 0; i < size; ++i) {
        if (B[i] != i + i) {
            cerr << "Test failed!" << endl;
            cerr << "Index: i is " << i << endl
                 << "B[i] = " << B[i] << endl;
            return -1;
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