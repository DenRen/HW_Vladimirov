#include "gtest/gtest.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>

#include "../bin/mycllib.h"
#include "../bin/cppl.hpp"
#include "../bin/other_func.hpp"

using std::cout;
using std::endl;

struct ocl_ctx_t {
    cl::Device device;
    cl::Context context;
    cl::CommandQueue cmd_queue;
    cl::Program program;

    ocl_ctx_t () :
        ocl_ctx_t ("kernels/sorter_v6.cl")
    {}

    ocl_ctx_t (std::string path_kernel) :
        device (hidra::DeviceProvider().getDefaultDevice ()),
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
    }
}; // struct ocl_ctx_t

template <typename T, typename Rand>
std::vector <T>
getRandFillVector (std::size_t size, Rand& rand) {
    std::vector <T> vec (size);
    for (auto& item : vec) {
        item = rand () % 50;
    }

    return vec; // RVO
}

template <typename T>
cl::Buffer
getBuffer (ocl_ctx_t& ctx, std::vector <T>& vec) {
    const std::size_t size_buffer = vec.size () * sizeof (T);
    cl::Buffer buffer (ctx.context,
                       CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                       size_buffer, vec.data ());
    return buffer;
}

template <typename T>
void
checkEqual (std::vector <T>& source_vec, const std::vector <T>& sorted_vec, bool dir) {
    auto copy_src_vec = source_vec;

    if (dir == false) {
        std::sort (source_vec.begin (), source_vec.end ());
    } else {
        std::sort (source_vec.begin (), source_vec.end (),
            [] (int& a, int& b) {
                return a > b;
            }
        );
    }

    ASSERT_TRUE (sorted_vec == source_vec)
        << "src: " << copy_src_vec << endl
        << "vec: " << sorted_vec   << endl
        << "ref: " << source_vec   << endl
        << "dir: " << dir          << endl;
}

static void
test_sort_intn (int n, ocl_ctx_t& ctx, std::string name_kernel_func,
                std::mt19937& mersenne, cl_uint dir) {
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
}

TEST (TEST_KERNEL, sort_int4) {
    ocl_ctx_t ctx;
    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const std::size_t repeat = 1 << 8;
    for (std::size_t i = 0; i < repeat; ++i) {
        test_sort_intn (4, ctx, "test_sort_int4", mersenne, i % 2);
    }
}

TEST (TEST_KERNEL, sort_int8) {
    ocl_ctx_t ctx;
    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const std::size_t repeat = 1 << 8;
    for (std::size_t i = 0; i < repeat; ++i) {
        test_sort_intn (8, ctx, "test_sort_int8", mersenne, i % 2);
    }
}

TEST (TEST_KERNEL, vector_sort_i4) {
    ocl_ctx_t ctx;
    std::random_device rd;
    std::mt19937 mersenne (rd ());
    mersenne.seed (3);

    const std::string name_kernel_func = "vector_sort_i4";
    const std::size_t repeat = 1;
    const std::size_t size = 1 << 10;
    const cl_int dir = 0;

    for (std::size_t i = 0; i < repeat; ++i) {
        cl::KernelFunctor <cl::Buffer, cl::LocalSpaceArg, cl_int>
            sort_intn (ctx.program, name_kernel_func);

        auto vec = getRandFillVector <int> (size, mersenne);
        cl::Buffer buffer = getBuffer (ctx, vec);

        auto save_vec = vec;
        
        const std::size_t num_item_data_block = size / (2 * sizeof (vec[0]));
        
        cl::NDRange global (num_item_data_block);
        cl::NDRange local (global);
        cl::EnqueueArgs args {ctx.cmd_queue, global, local};
        cl::LocalSpaceArg local_buffer { size * sizeof (vec[0]) };

        sort_intn (args, buffer, local_buffer, dir);

        cl::copy (ctx.cmd_queue, buffer, vec.data (), vec.data () + vec.size ());
        // cout  << "res: " << vec << endl;

        checkEqual (save_vec, vec, dir);
    }
}