#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>

#include "../../bin/cppl.hpp"
#include "../../bin/other_func.hpp"

using std::cout;
using std::endl;

struct ocl_ctx_t {
    cl::Device device;
    cl::Context context;
    cl::CommandQueue cmd_queue;
    cl::Program program;

    ocl_ctx_t () :
        ocl_ctx_t ("kernels/sorter.cl")
    {} // ocl_ctx_t ()

    ocl_ctx_t (std::string path_kernel) : // The path to the kerenl (.cl) file
        device (hidra::DeviceProvider ().getDefaultDevice ()),
        context (device),
        cmd_queue (context),
        program (context, readSource (path_kernel))
    {
        try {
            program.build ("-DENABLE_TESTING");
        } catch (cl::Error& exc) {
            cl_int buildError = CL_SUCCESS;

            auto buildInfo = program.getBuildInfo <CL_PROGRAM_BUILD_LOG> (&buildError);

            for (const auto& pair : buildInfo) {
                auto device = pair.first.getInfo <CL_DEVICE_NAME> ();
                auto build_error_msg = pair.second;
                std::cerr << "Device: " << device << std::endl;
                std::cerr << "Build msg: " << build_error_msg << std::endl;
            }

            throw;
        }
    } // ocl_ctx_t (std::string path_kernel)
}; // struct ocl_ctx_t

template <typename T>
cl::Buffer
getBuffer (ocl_ctx_t& ctx,        // The context where the buffer will be created
           std::vector <T>& vec)  // Data for buffer
{
    const std::size_t size_buffer = vec.size () * sizeof (T);
    cl::Buffer buffer (ctx.context,
                       CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                       size_buffer, vec.data ());
    return buffer;
} // getBuffer (ocl_ctx_t& ctx, std::vector <T>& vec)

static void
test_sort_intn (int n,                        // Number ints in intn type (n == 4 => int4)
                ocl_ctx_t& ctx,               // Context where will be test
                std::string name_kernel_func, // Name kernel function for testing
                std::mt19937& mersenne,       // Random generator
                cl_uint dir)                  // Direction sort (0 -> /, 1 -> \)
{
    cl::KernelFunctor <cl::Buffer, cl_int> sort_intn (ctx.program, name_kernel_func);

    auto vec = getRandFillVector <int> (n, mersenne);
    cl::Buffer buffer = getBuffer (ctx, vec);

    auto save_vec = vec;

    cl::NDRange global (1);
    cl::NDRange local (1);
    cl::EnqueueArgs args {ctx.cmd_queue, global, local};
    sort_intn (args, buffer, dir);

    cl::copy (ctx.cmd_queue, buffer, vec.data (), vec.data () + vec.size ());

    checkEqual (save_vec, vec, dir);
} // test_sort_intn (int n, ocl_ctx_t& ctx, std::string name_kernel_func,
  //                 std::mt19937& mersenne, cl_uint dir)

TEST (TEST_KERNEL, sort_int4) {
    ocl_ctx_t ctx;
    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const std::size_t repeat = 1 << 8;
    for (std::size_t i = 0; i < repeat; ++i) {
        test_sort_intn (4, ctx, "test_sort_int4", mersenne, i % 2);
    }
} // TEST (TEST_KERNEL, sort_int4)

TEST (TEST_KERNEL, sort_int8) {
    ocl_ctx_t ctx;
    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const std::size_t repeat = 1 << 8;
    for (std::size_t i = 0; i < repeat; ++i) {
        test_sort_intn (8, ctx, "test_sort_int8", mersenne, i % 2);
    }
} // TEST (TEST_KERNEL, sort_int8)