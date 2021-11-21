#pragma once

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define PRINT_LOCATION \
    printf ("In file: %s -> function: %s -> line: %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);

#define RETURN_ERROR    \
    do {                \
        PRINT_LOCATION; \
        return -1;      \
    } while (0)

#define CHECK_CLERROR(result)                       \
    do {                                            \
        int _result = (result);                     \
        if (_result != CL_SUCCESS) {                \
            printf ("Error code: %d\n", _result);   \
            RETURN_ERROR;                           \
        }                                           \
    } while (0)

#define CHECK_ERROR(result)     \
    do {                        \
        int _result = (result); \
        if (_result == false)   \
            RETURN_ERROR;       \
    } while (0)

#define CHECK(result) CHECK_ERROR (result != -1)

int print_all_platforms_with_all_devices ();
int sum_array (int* A, int* B, int* res, size_t size);

int learn_buf ();
int test_summ_array ();

/*
void step_0 () {
    const int NWTIMES = 512;
    const char* source =
    "kernel void memset (global uint* dst) {"
    "   dst[get_global_id (0)] = get_global_id (0);"
    "}";


    cl_platform_id platform;
    clGetPlatformIDs (1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs (platform,
                    CL_DEVICE_TYPE_GPU,
                    1,
                    &device, NULL);

    cl_context context = clCreateContext (NULL, 1, &device, NULL, NULL, NULL);

    cl_command_queue queue = clCreateCommandQueue (context,
                                                   device,
                                                   0, NULL);

    cl_program program = clCreateProgramWithSource (context, 1,
                                                    &source, NULL, NULL);
    clBuildProgram (program, 1, &device,  NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel (program, "memset", NULL);

    cl_mem buffer = clCreateBuffer (context,
                                    CL_MEM_WRITE_ONLY,
                                    NWTIMES * sizeof (cl_uint),
                                    NULL, NULL);

    size_t global_work_size = NWTIMES;
    clSetKernelArg (kernel, 0, sizeof (buffer), &buffer);

    clEnqueueNDRangeKernel (queue, kernel, 1, NULL, &global_work_size,
                            NULL, 0, NULL, NULL);
    clFinish (queue);

    cl_uint* ptr = (cl_uint*) clEnqueueMapBuffer (queue,
                                                  buffer,
                                                  CL_TRUE,
                                                  CL_MAP_READ,
                                                  0,
                                                  NWTIMES * sizeof (cl_uint),
                                                  0, NULL, NULL, NULL);

    for (int i = 0; i < NWTIMES; ++i) {
        printf ("%d: %d\n", i, ptr[i]);
    }
}*/