#include <CL/cl.h>
#include <stdio.h>
#include "mycllib.h"

int test_summ_array ();

int learn_buf ();

int
main (int argc, char* argv[]) {
    // if (print_all_platforms_with_all_devices () == -1) {
    //     printf ("Error!\n");
    // }

    // learn_buf ();
    test_summ_array ();
}

static int
work_with_buffer (cl_context context, cl_command_queue queue) {
    const size_t size = 128;
    int A[size], B[size];

    const size_t size_A = size * sizeof (A[0]);

    for (size_t i = 0; i < size; ++i) {
        A[i] = 17 * i - 3;
        B[i] = 0;
    }

    cl_int error_code;
    cl_mem buffer = clCreateBuffer (context, CL_MEM_READ_WRITE, size_A,
                                    nullptr, &error_code);
    CHECK_CLERROR (error_code);

    CHECK_CLERROR (clEnqueueWriteBuffer (queue, buffer, CL_TRUE, 0, size_A,
                                         A, 0, nullptr, nullptr));

    CHECK_CLERROR (clEnqueueReadBuffer (queue, buffer, CL_TRUE, 0, size_A,
                                        B, 0, nullptr, nullptr));

    for (size_t i = 0; i < size; ++i) {
        if (A[i] != B[i]) {
            printf ("Test failed! Iter: %d\n", i);
            printf ("A[i]: %d, B[i]: %d\n", A[i], B[i]);
            return 0;
        }
    }

    printf ("Buffer copyed correctly!\n");

    return 0;
}

int
learn_buf () {
    cl_uint num_platforms;
    cl_platform_id platform;
    CHECK_CLERROR (clGetPlatformIDs (1, &platform, &num_platforms));
    if (num_platforms == 0) {
        printf ("No platforms!");
        return -1;
    }

    cl_uint num_devices;
    cl_device_id device;
    CHECK_CLERROR (clGetDeviceIDs (platform, CL_DEVICE_TYPE_GPU, 1, &device, &num_devices));
    if (num_devices == 0) {
        printf ("No devices!");
        return -1;
    }

    cl_int error_code;
    cl_context context = clCreateContext (nullptr,
                                          1, &device,
                                          nullptr,
                                          nullptr,
                                          &error_code);
    CHECK_CLERROR (error_code);

    cl_command_queue queue = clCreateCommandQueueWithProperties (context, device,
                                                                 0ull, &error_code);
    CHECK_CLERROR (error_code);

    CHECK (work_with_buffer (context, queue));

    CHECK_CLERROR (clFlush (queue));
    CHECK_CLERROR (clFinish (queue));

    CHECK_CLERROR (clReleaseCommandQueue (queue));
    CHECK_CLERROR (clReleaseContext (context));

    return 0;
}

int
test_summ_array () {
    const size_t size = 128;
    int A[size], B[size], res[size], ref_res[size];

    for (size_t i = 0; i < size; ++i) {
        A[i] = 17 * i - 3;
        B[i] = 3 * i * i - 80 * i + 4;

        res[i] = -777; // Poison
        ref_res[i] = A[i] + B[i];
    }

    if (sum_array (A, B, res, size) == -1) {
        printf ("Error in sum_array!\n");
        return 0;
    }

    for (size_t i = 0; i < size; ++i) {
        if (res[i] != ref_res[i]) {
            printf ("Test failed! Iter: %d\n", i);
            printf ("res[i]: %d, ref_res[i]: %d\n", res[i], ref_res[i]);
            return 0;
        }
    }

    printf ("Test passed!\n");

    return 0;
}