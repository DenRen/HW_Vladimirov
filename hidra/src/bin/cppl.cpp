#include "cppl.hpp"
#include "other_func.hpp"

#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <random>
#include <chrono>

namespace hidra {

DeviceProvider::DeviceProvider (cl_device_type device_type, // Device type (CPU, GPU, ...)
                                std::string_view version) : // Version of OpenCL (Ex.: OpenCL 3.0)
    defualt_device_ (nullptr)
{
    std::vector <cl::Platform> platforms;
    cl::Platform::get (&platforms);

    if (platforms.size () == 0) {
        throw cl::Error (0, "Platforms not found");
    }

    for (const auto& platform : platforms) {
        auto platform_version = platform.getInfo <CL_PLATFORM_VERSION> ();
        auto platform_name = platform.getInfo <CL_PLATFORM_NAME> ();

        if (platform_version.find (version) != decltype (platform_version)::npos &&
            platform_name.find ("NVIDIA") != decltype (platform_name)::npos) {
            std::vector <cl::Device> devices;

            platform.getDevices (device_type, &devices);
            if (devices.size () != 0) {
                default_platform_ = platform;
                defualt_device_ = devices[0];
            }
        }
    }

    if (defualt_device_ () == nullptr) {
        std::string err_msg ("Device with version");
        err_msg += version;
        err_msg += "not found in platform!";

        throw cl::Error (0, err_msg.c_str ());
    }
} // DeviceProvider::DeviceProvider (cl_device_type device_type, std::string_view version)

cl::Platform
DeviceProvider::getDefaultPlatform () const {
    return default_platform_;
} // DeviceProvider::getDefaultPlatform () const
cl::Device DeviceProvider::getDefaultDevice () const {
    return defualt_device_;
} // DeviceProvider::getDefaultDevice () const

std::string
DeviceProvider::getDefaultPlatformName () const {
    return default_platform_.getInfo <CL_PLATFORM_NAME> ();
} // DeviceProvider::getDefaultPlatformName () const
std::string
DeviceProvider::getDefaultDeviceName () const {
    return defualt_device_.getInfo <CL_DEVICE_NAME> ();
} // DeviceProvider::getDefaultPlatformName () const

static int
round_down_pow2 (int n) // Number to be rounded
{
    int i = 0;
    for (; n != 0; ++i) {
        n >>= 1;
    }

    return 1 << (i - 1);
} // round_down_pow2 (int n)

static cl::Program
buildProgram (cl::Context context,           // The context in which the program will be built
              std::string name_kernel_func)  // Name kernel function
{
    cl::Program program (context, readSource (name_kernel_func));
    try {
        std::string_view options = "-cl-unsafe-math-optimizations \
                                    -cl-fast-relaxed-math\
                                    -cl-mad-enable";
        program.build (options.data ());
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

    return program;
} // buildProgram (cl::Context context, std::string name_kernel_func)

Sorter::Sorter (cl::Device device) : // The device on which the sorter will work
    device_ (device),
    context_ (device_),
    cmd_queue_ (context_),
    program_ (buildProgram (context_, "kernels/sorter.cl")),
    bitonic_sort_local_      (program_, "i4_bitonic_sort_local"),
    bitonic_sort_full_local_ (program_, "i4_bitonic_sort_full_local"),
    bitonic_merge_global_    (program_, "i4_bitonic_merge_global"),
    bitonic_merge_local_     (program_, "i4_bitonic_merge_local"),
    max_group_size_ (device.getInfo <CL_DEVICE_MAX_WORK_GROUP_SIZE> ())
{
    const std::size_t local_size = device.getInfo <CL_DEVICE_LOCAL_MEM_SIZE> ();

    using data_type = cl_int4;
    const std::size_t max_possible_group_size = local_size / (2 * sizeof (data_type));
    if (max_group_size_ > max_possible_group_size) {
        max_group_size_ = round_down_pow2 (max_possible_group_size);
    }

    max_group_size_ = round_down_pow2 (max_group_size_);
} // Sorter::Sorter (cl::Device device)

template <>
void
Sorter::vect_sort <int> (int* input_data,       // Data to be sorted
                         std::size_t data_size, // The size of the data in the number of int
                         uint dir)              // Direction sort (1 -> /, 0 -> \)
{
    using data_type = cl_int4;

    // If array size less then minimum compared item (int4) * 2
    if (data_size < 2 * sizeof (data_type) / sizeof (input_data[0])) {
        std::sort (input_data, input_data + data_size, [dir = dir] (auto& lhs, auto& rhs) {
            return (lhs > rhs) ^ dir;
        });
        return;
    }

    data_size /= sizeof (data_type) / sizeof (input_data[0]);
    data_type* data = reinterpret_cast <data_type*> (input_data);

    uint l_buf_size = 2 * max_group_size_;

    cl::LocalSpaceArg local_buf { .size_ = sizeof (data_type) * l_buf_size };

    cl::Buffer buffer (context_, CL_MEM_READ_WRITE, data_size * sizeof (data_type));
    cl::copy (cmd_queue_, data, data + data_size, buffer);

    if (data_size <= l_buf_size) {
        uint threadCount = data_size / 2;

        cl::NDRange global (threadCount);
        cl::NDRange local (threadCount);
        cl::EnqueueArgs args {cmd_queue_, global, local};

        bitonic_sort_local_ (args, local_buf, buffer, data_size, dir);
        cl::copy (cmd_queue_, buffer, data, data + data_size);
    } else {
        uint threadCount = l_buf_size / 2;
        uint blockCount = data_size / l_buf_size;

        cl::NDRange global (blockCount * threadCount);
        cl::NDRange local (threadCount);
        cl::EnqueueArgs args {cmd_queue_, global, local};

        bitonic_sort_full_local_ (args, local_buf, buffer);

        for (uint size = 2 * l_buf_size; size <= data_size; size <<= 1)
        for (unsigned stride = size / 2; stride > 0; stride >>= 1)
            if (stride >= l_buf_size) {
                bitonic_merge_global_ (args, buffer, data_size, size, stride, dir);
            } else {
                bitonic_merge_local_ (args, local_buf, buffer, data_size, size, dir);
                break;
            }

        cl::copy (cmd_queue_, buffer, data, data + data_size);
    }
} // Sorter::vect_sort <int> (int* input_data, std::size_t data_size, uint dir)

void
testSpeed (unsigned pow2_begin, unsigned pow2_end) {
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const size_t min_size_arr = 8 * 1 << pow2_begin;
    const size_t max_size_arr = 8 * 1 << pow2_end;
    const size_t repeat = 10;

    for (std::size_t size_arr = min_size_arr; size_arr <= max_size_arr; size_arr *= 2) {
        // Generate vectors
        std::vector <std::vector <int>> vecs;
        vecs.reserve (repeat);
        for (std::size_t i = 0; i < repeat; ++i) {
            vecs.push_back (getRandFillVector <int> (size_arr, mersenne));
        }
        auto copy_vecs = vecs;

        // Test GPU
        auto gpu_begin = std::chrono::high_resolution_clock::now ();
        for (auto& vec : vecs) {
            sorter.vect_sort (vec);
        }
        auto gpu_end = std::chrono::high_resolution_clock::now ();

        // Test CPU
        auto cpu_begin = std::chrono::high_resolution_clock::now ();
        for (auto& vec : copy_vecs) {
            std::sort (vec.begin (), vec.end ());
        }
        auto cpu_end = std::chrono::high_resolution_clock::now ();

        for (std::size_t i = 0; i < repeat; ++i) {
            if (copy_vecs[i] != vecs[i]) {
                std::cerr << "Error! copy_vecs_cpu[i] != vecs[i]" << std::endl;
            }
        }

        auto to_ns = [] (auto time) {
            return std::chrono::duration_cast <std::chrono::nanoseconds> (time);
        };
        auto gpu_time = to_ns (gpu_end - gpu_begin) / repeat / 1000;
        auto cpu_time = to_ns (cpu_end - cpu_begin) / repeat / 1000;

        std::cout << "Size array: " << std::setw (10) << size_arr
                  << std::setw (12) << "C/G: " << (double)cpu_time.count () / gpu_time.count () << std::endl
                  << tab << "time GPU: " <<  std::setw (10) << gpu_time.count () << " mks" << std::endl
                  << tab << "time CPU: " <<  std::setw (10) << cpu_time.count () << " mks" << std::endl;

    }
} // testSpeed (unsigned pow2_begin, unsigned pow2_end)

} // namespace hidra