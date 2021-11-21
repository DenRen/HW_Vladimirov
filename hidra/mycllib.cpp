#include "mycllib.h"

#include <cstdio>

static int
print_platform_param (cl_platform_id platform,
                      cl_platform_info param_name,
                      const char* format_string)
{
    size_t size_info = 0;

    CHECK_CLERROR (clGetPlatformInfo (
        platform, param_name, 0, nullptr, &size_info
    ));

    uint8_t platform_info[size_info] = {};
    CHECK_CLERROR (clGetPlatformInfo (
        platform, param_name, size_info, platform_info, nullptr
    ));

    printf (format_string, platform_info);

    return 0;
}

#define PRINT(platform, param, str) \
    CHECK (print_platform_param (platform, param, str))

static int
print_platform_info (cl_platform_id platform,
                     bool with_extensions = false,
                     bool with_host_timer_res = false)
{
    PRINT (platform, CL_PLATFORM_PROFILE,    "\tProfile: %s\n");
    PRINT (platform, CL_PLATFORM_VERSION,    "\tVersion: %s\n");
    PRINT (platform, CL_PLATFORM_NAME,       "\tName:    %s\n");
    PRINT (platform, CL_PLATFORM_VENDOR,     "\tVendor:  %s\n");

    if (with_extensions)
        PRINT (platform, CL_PLATFORM_EXTENSIONS, "\tExtensions: %s\n");

    #ifdef CL_PLATFORM_HOST_TIMER_RESOLUTION
    if (with_host_timer_res)
        PRINT (platform, CL_PLATFORM_HOST_TIMER_RESOLUTION, "\tHost timer resolution: %s\n");
    #endif

    return 0;
}
#undef PRINT

static int
print_device_param (cl_device_id device,
                    cl_device_info param_name,
                    const char* format_string)
{
    size_t size_info = 0;

    CHECK_CLERROR (clGetDeviceInfo (
        device, param_name, 0, nullptr, &size_info
    ));

    uint8_t platform_info[size_info] = {};
    CHECK_CLERROR (clGetDeviceInfo (
        device, param_name, size_info, platform_info, nullptr
    ));

    printf (format_string, platform_info);

    return 0;
}

#define PRINT(device, param, str) \
    CHECK (print_device_param (device, param, str))

static int
print_device_info (cl_device_id device,
                   bool with_extensions = false)
{
    #define INDENT "\t\t"

    PRINT (device, CL_DEVICE_NAME,    INDENT "Name:    %s\n");
    PRINT (device, CL_DEVICE_VERSION, INDENT "Version: %s\n");
    PRINT (device, CL_DEVICE_PROFILE, INDENT "Profile: %s\n");
    PRINT (device, CL_DEVICE_VENDOR,  INDENT "Vendor:  %s\n");

    cl_device_type device_type;
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_TYPE, sizeof (device_type), &device_type, nullptr));
    printf (INDENT "Type: %zu\n", device_type);

    cl_uint vendor_id, max_compute_units, max_work_item_dims;
    size_t max_group_work_size;
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_VENDOR_ID,                sizeof (cl_uint), &vendor_id,           nullptr));
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_MAX_COMPUTE_UNITS,        sizeof (cl_uint), &max_compute_units,   nullptr));
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof (cl_uint), &max_work_item_dims,  nullptr));
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_MAX_WORK_GROUP_SIZE,      sizeof (size_t),  &max_group_work_size, nullptr));
    printf (INDENT "Vendor id: %u\n\n", vendor_id);
    printf (INDENT "Max compute units: %u\n", max_compute_units);
    printf (INDENT "Max group work size: %zu\n", max_group_work_size);
    printf (INDENT "Max work item dimensions: %u\n", max_work_item_dims);

    size_t max_work_item_sizes[max_work_item_dims];
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_MAX_WORK_ITEM_SIZES,
                                    max_work_item_dims * sizeof (max_work_item_sizes), &max_work_item_sizes, nullptr));
    printf (INDENT "Max work item sizes:");
    for (int i = 0; i < max_work_item_dims; ++i) {
        printf (" %zu", max_work_item_sizes[i]);
    }
    printf ("\n");

    cl_uint max_clock_freq;
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof (cl_uint), &max_clock_freq, nullptr));
    printf (INDENT "Max clock frequency: %u\n\n", max_clock_freq);

    cl_ulong global_mem_size, local_mem_size, glob_mem_cache_size, glob_mem_cacheline_size;
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_GLOBAL_MEM_SIZE,           sizeof (cl_ulong), &global_mem_size,         nullptr));
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_LOCAL_MEM_SIZE,            sizeof (cl_ulong), &local_mem_size,          nullptr));
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,     sizeof (cl_ulong), &glob_mem_cache_size,     nullptr));
    CHECK_CLERROR (clGetDeviceInfo (device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof (cl_ulong), &glob_mem_cacheline_size, nullptr));
    printf (INDENT "Global memory size: %lu Mbyte\n", global_mem_size / 1024 / 1024);
    printf (INDENT "Local memory size: %lu Kbyte\n", local_mem_size / 1024);
    printf (INDENT "Global memory cache size: %lu Kbyte\n", glob_mem_cache_size / 1024);
    printf (INDENT "Global memory cacheline size: %lu byte\n", glob_mem_cacheline_size);

    if (with_extensions)
        PRINT (device, CL_DEVICE_EXTENSIONS, "\t\tExtensions: %s\n");

    return 0;
}
#undef PRINT

static int
print_devices_info (cl_platform_id platform)
{
    cl_uint num_devices = 0;
    CHECK_CLERROR (clGetDeviceIDs (platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices));

    if (num_devices == 0) {
        printf ("No device\n");
    }

    cl_device_id devices[num_devices] = {};
    CHECK_CLERROR (clGetDeviceIDs (platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL));

    printf ("\n\tNumber devices: %d\n", num_devices);

    for (cl_uint num_device = 0; num_device < num_devices; ++num_device) {
        printf ("\tDevice %d:\n", num_device);
        CHECK (print_device_info (devices[num_device]));
    }

    return 0;
}

static int
print_platforms_with_devices (cl_platform_id* platforms,
                              unsigned number)
{
    CHECK_ERROR (platforms != NULL);

    for (unsigned i = 0; i < number; ++i) {
        printf ("Platform -> %d:\n", i);
        CHECK (print_platform_info (platforms[i]));
        CHECK (print_devices_info (platforms[i]));
    }

    return 0;
}

int
print_all_platforms_with_all_devices () {
    // Get platforms
    cl_uint num_platforms = 0;
    CHECK_CLERROR (clGetPlatformIDs (0, NULL, &num_platforms));

    if (num_platforms == 0) {
        printf ("No platforms\n");
        return 0;
    }

    cl_platform_id platforms[num_platforms] = {};
    CHECK_CLERROR (clGetPlatformIDs (num_platforms, platforms, NULL));

    // Print platforms and them devices
    print_platforms_with_devices (platforms, num_platforms);

    return 0;
}

#define STRINGIFY(obj) #obj

int
sum_array (int* A, int* B, int* res, size_t size) {
    CHECK_ERROR (A != nullptr && B != nullptr && res != nullptr);

    if (size == 0) {
        return 0;
    }

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

    const char* vakernel = STRINGIFY (
        __kernel void vector_add (__global int* A, __global int* B, __global int* res) {
            int i = get_global_id (0);
            res[i] = A[i] + B[i];
        }
    );

    cl_program program = clCreateProgramWithSource (context,
                                                    1, &vakernel, nullptr,
                                                    &error_code);
    CHECK_CLERROR (error_code);

    CHECK_CLERROR (clBuildProgram (program,
                                   1, &device,
                                   nullptr, nullptr, nullptr));
    cl_kernel kernel = clCreateKernel (program,
                                       "vector_add",
                                       &error_code);
    CHECK_CLERROR (error_code);

    // Fill buffer
    const size_t size_buffer = size * sizeof (A[0]);
    cl_mem buffer_A = clCreateBuffer (context, CL_MEM_READ_ONLY, size_buffer, nullptr, &error_code);
    CHECK_CLERROR (error_code);
    cl_mem buffer_B = clCreateBuffer (context, CL_MEM_READ_ONLY, size_buffer, nullptr, &error_code);
    CHECK_CLERROR (error_code);
    cl_mem buffer_R = clCreateBuffer (context, CL_MEM_WRITE_ONLY, size_buffer, nullptr, &error_code);
    CHECK_CLERROR (error_code);

    CHECK_CLERROR (clEnqueueWriteBuffer (queue, buffer_A, CL_TRUE,
                                         0, size_buffer, A,
                                         0, nullptr, nullptr));
    CHECK_CLERROR (clEnqueueWriteBuffer (queue, buffer_B, CL_TRUE,
                                         0, size_buffer, B,
                                         0, nullptr, nullptr));

    CHECK_CLERROR (clSetKernelArg (kernel, 0, sizeof (cl_mem*), &buffer_A));
    CHECK_CLERROR (clSetKernelArg (kernel, 1, sizeof (cl_mem*), &buffer_B));
    CHECK_CLERROR (clSetKernelArg (kernel, 2, sizeof (cl_mem*), &buffer_R));

    CHECK_CLERROR (clEnqueueNDRangeKernel (queue, kernel,
                                           1, nullptr, &size, nullptr,
                                           0, nullptr, nullptr));

    CHECK_CLERROR (clEnqueueReadBuffer (queue, buffer_R, CL_TRUE,
                                        0, size_buffer, res,
                                        0, nullptr, nullptr));

    CHECK_CLERROR (clFlush (queue));
    CHECK_CLERROR (clFinish (queue));

    CHECK_CLERROR (clReleaseMemObject (buffer_R));
    CHECK_CLERROR (clReleaseMemObject (buffer_B));
    CHECK_CLERROR (clReleaseMemObject (buffer_A));

    CHECK_CLERROR (clReleaseKernel (kernel));
    CHECK_CLERROR (clReleaseProgram (program));
    CHECK_CLERROR (clReleaseCommandQueue (queue));
    CHECK_CLERROR (clReleaseContext (context));

    CHECK_CLERROR (clReleaseDevice (device));

    return 0;
}

#undef STRINGIFY


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
            printf ("Test failed! Iter: %zu\n", i);
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
            printf ("Test failed! Iter: %zu\n", i);
            printf ("res[i]: %d, ref_res[i]: %d\n", res[i], ref_res[i]);
            return 0;
        }
    }

    printf ("Test passed!\n");

    return 0;
}