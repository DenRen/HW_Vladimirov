#include "CL/cl.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    cl_uint num;
    cl_platform_id* ids;
} platforms_t;

void cl_process_error (cl_int ret, const char* file, int line) {
    const char* cause = "unknown";
    switch (ret) {
        case CL_SUCCESS:
            return;
        case CL_INVALID_VALUE:
            cause = "invalide value";
            break;
        case CL_INVALID_DEVICE:
            cause = "invalide device";
            break;
        case CL_OUT_OF_HOST_MEMORY:
            cause = "out of host memory";
            break;
        case CL_OUT_OF_RESOURCES:
            cause = "out of resources";
            break;
    }

    fprintf (stderr, "Error %s at %s:%d code%d\n", cause, file, line, ret);

    abort ();
}

#define CHECK_ERR(ret) cl_process_error (ret, __FILE__, __LINE__)

platforms_t get_platforms () {
    cl_int ret;
    platforms_t platforms;

    ret = clGetPlatformIDs (0, NULL, &platforms.num);
    CHECK_ERR (ret);

    assert (platforms.num > 0);
    platforms.ids = (cl_platform_id*) calloc (platforms.num, sizeof (cl_platform_id));
    
    ret = clGetPlatformIDs (platforms.num, platforms.ids, NULL);
    CHECK_ERR (ret);

    return platforms;
}

enum { STRING_BUFSIZE = 1024 };

void print_one_platform_info (cl_platform_id platform_id,
                              cl_platform_info platform_info,
                              const char* platform_info_name)
{
    char buf[STRING_BUFSIZE] = "";

    cl_int ret = clGetPlatformInfo (platform_id, platform_info, sizeof (buf), buf, NULL);
    CHECK_ERR (ret);
    printf ("%s%s\n", platform_info_name, buf);
}

void print_platform_info (cl_platform_id platform_id) {
    
    print_one_platform_info (platform_id, CL_PLATFORM_NAME,       "Device:  ");
    print_one_platform_info (platform_id, CL_PLATFORM_VERSION,    "Version: ");
    print_one_platform_info (platform_id, CL_PLATFORM_PROFILE,    "Profile: ");
    print_one_platform_info (platform_id, CL_PLATFORM_VENDOR,     "Vendor:  ");

    char buf[STRING_BUFSIZE] = "";
    cl_int ret = clGetPlatformInfo (platform_id, CL_PLATFORM_EXTENSIONS, sizeof (buf), buf, NULL);
    CHECK_ERR (ret);

    printf ("Extensions:\n");

    char* token = strtok (buf, " ");
    if (token != NULL) {
        printf ("\t%s\n", token);

        while ((token = strtok (NULL, " ")) != NULL)
            printf ("\t%s\n", token);
    }
}

void print_device_one_info (cl_device_id device_id,
                            cl_device_info device_info,
                            const char* device_info_name)
{
    char buf[STRING_BUFSIZE] = "";
    cl_int ret = clGetDeviceInfo (device_id, device_info, sizeof (buf), buf, NULL);
    CHECK_ERR (ret);

    printf ("%s%s\n", device_info_name, buf);
}

void print_device_info (cl_device_id device_id) {
    print_device_one_info (device_id, CL_DEVICE_NAME,                "Name:                ");
    print_device_one_info (device_id, CL_DEVICE_VERSION,             "Version:             ");

    cl_int ret = 0;
    cl_uint ubuf = 0;

    ret = clGetDeviceInfo (device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(ubuf), &ubuf, NULL);
    CHECK_ERR(ret);
    printf("Max units:           %u\n", ubuf);

    ret = clGetDeviceInfo (device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(ubuf), &ubuf, NULL);
    CHECK_ERR(ret);
    printf("Max dimensions:      %u\n", ubuf);

    size_t* psbuf = malloc(sizeof(size_t) * ubuf);
    ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * ubuf, psbuf, NULL);
    CHECK_ERR(ret);

    printf("Max work item sizes: ");
    for (cl_uint i = 0; i != ubuf; ++i)
        printf("%u ", (unsigned)psbuf[i]);
    printf("\n");
    free(psbuf);

    size_t sbuf = 0;
    ret = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof (sbuf), &sbuf, NULL);
    CHECK_ERR(ret);
    printf("Max work group size: %u\n", (unsigned)sbuf);
    
    cl_bool avail = 0;
    ret = clGetDeviceInfo (device_id, CL_DEVICE_COMPILER_AVAILABLE, sizeof (avail), &avail, NULL);
    CHECK_ERR (ret);
    printf ("Compiler:            %savailable\n", avail ? "" : "not ");

    ret = clGetDeviceInfo (device_id, CL_DEVICE_LINKER_AVAILABLE, sizeof (avail), &avail, NULL);
    CHECK_ERR (ret);
    printf ("Linker:              %savailable\n", avail ? "" : "not ");
}

typedef struct {
    cl_uint num;
    cl_device_id* ids;
} devices_t;

devices_t get_devices (cl_platform_id platform_id) {
    devices_t devices;

    cl_int ret = clGetDeviceIDs (platform_id, CL_DEVICE_TYPE_ALL, 0,  NULL, &devices.num);
    CHECK_ERR (ret);

    devices.ids = (cl_device_id*) calloc (devices.num, sizeof (cl_device_id));
    assert (devices.ids != NULL);

    ret = clGetDeviceIDs (platform_id, CL_DEVICE_TYPE_ALL, devices.num, devices.ids, NULL);
    CHECK_ERR (ret);

    return devices;
}

int main (int argc, char* argv[]) {
    platforms_t platforms = get_platforms ();

    for (int i = 0; i < platforms.num; ++i) {
        print_platform_info (platforms.ids[i]);
        printf ("\n");

        devices_t devices = get_devices (platforms.ids[i]);

        for (int idev = 0; idev < devices.num; ++idev)
            print_device_info (devices.ids[idev]);

        free (devices.ids);
    }

    free (platforms.ids);
}