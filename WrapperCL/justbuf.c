#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 250
#endif

#include "CL/cl.h"
#include <assert.h>
#include <stdio.h>

static void cl_notify_fn (const char* errinfo, const void* private_info,
                          size_t cb, void* user_data);

void cl_process_error (cl_int ret, const char* file, int line);

#define CHECK_ERR(ret) cl_process_error (ret, __FILE__, __LINE__)

typedef struct {
    cl_context context;
    cl_command_queue queue;
} ocl_ctx_t;

void process_buffers (ocl_ctx_t* ct);

int main () {
    ocl_ctx_t ct;
    cl_int ret = 0;

    ct.context = clCreateContextFromType(NULL, CL_DEVICE_TYPE_GPU, &cl_notify_fn,
                                         NULL, &ret);
    CHECK_ERR (ret);

    size_t num_devs = 0;
    ret = clGetContextInfo (ct.context, CL_CONTEXT_DEVICES, 0, NULL, &num_devs);
    CHECK_ERR (ret);

    cl_device_id* device_id = (cl_device_id*) calloc (num_devs, sizeof (cl_device_id));
    ret = clGetContextInfo (ct.context, CL_CONTEXT_DEVICES, num_devs, device_id, NULL);
    CHECK_ERR (ret);

#if CL_TARGET_OPENCL_VERSION < 200
    ct.queue = clCreateCommandQueue (ct.context, device_id[0], 0, &ret);
#else
    ct.que = clCreateCommandQueueWithProperties(ct.ctx, devs[0], NULL, &ret);
#endif
    CHECK_ERR (ret);

    process_buffers (&ct);

    ret = clFlush (ct.queue);
    CHECK_ERR (ret);

    ret = clFinish (ct.queue);
    CHECK_ERR (ret);

    ret = clReleaseCommandQueue (ct.queue);
    CHECK_ERR (ret);

    ret = clReleaseContext (ct.context);
    CHECK_ERR (ret);

    free (device_id);
}

enum { BUFSIZE = 128 };

void process_buffers (ocl_ctx_t* pct) {
    int A[BUFSIZE];
    int B[BUFSIZE] = {0};

    for (int i = 0; i < BUFSIZ; ++i)
        A[i] = i * i * i;

    cl_int ret = 0;
    cl_mem ocl_buf = clCreateBuffer (pct->context, CL_MEM_READ_WRITE, BUFSIZE * sizeof (int),
                                     NULL, &ret);
    CHECK_ERR (ret);

    ret = clEnqueueWriteBuffer (pct->queue, ocl_buf, CL_TRUE, 0, BUFSIZE * sizeof (int),
                                A, 0, NULL, NULL);
    CHECK_ERR (ret);

    ret = clEnqueueReadBuffer (pct->queue, ocl_buf, CL_TRUE, 0, BUFSIZE * sizeof (int),
                               B, 0, NULL, NULL);
    CHECK_ERR (ret);

    for (int i = 0; i < BUFSIZE; ++i)
        if (B[i] != i * i * i) {
            fprintf(stderr, "Error: B[%d] != %d * %d\n", i, i, i);
            abort();
        }

    fprintf(stdout, "%s\n", "Everything is correct");

    clReleaseMemObject (ocl_buf);
}

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

static void cl_notify_fn (const char* errinfo, const void* private_info,
                          size_t cb, void* user_data) {
    fprintf(stderr, "Context error: %s\n", errinfo);
    abort();
}