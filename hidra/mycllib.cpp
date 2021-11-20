#include "mycllib.h"

#include <cstdio>

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
    PRINT (device, CL_DEVICE_NAME,      "\t\tName:    %s\n");
    PRINT (device, CL_DEVICE_VERSION,   "\t\tVersion: %s\n");
    PRINT (device, CL_DEVICE_PROFILE,   "\t\tProfile: %s\n");
    PRINT (device, CL_DEVICE_VENDOR,    "\t\tVendor:  %s\n");

    // todo: add type, ...

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