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