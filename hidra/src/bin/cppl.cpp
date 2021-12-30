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
#include <bit>
#include <map>
#include <tuple>

namespace hidra {

std::vector <std::tuple <std::string, cl::Platform, cl::Device>>
getDeviceFromAllPlatforms (cl_device_type device_type) {
    std::vector <cl::Platform> platforms;
    cl::Platform::get (&platforms);

    if (platforms.size () == 0) {
        throw cl::Error (0, "Platforms not found");
    }

    std::vector <std::tuple <std::string, cl::Platform, cl::Device>> vendorDevices;
    vendorDevices.reserve (platforms.size ());

    for (const auto& platform : platforms) {

        std::vector <cl::Device> devices;
        platform.getDevices (device_type, &devices);
        if (devices.size () == 0) {
            continue;
        }

        auto platformName = platform.getInfo <CL_PLATFORM_NAME> ();
        vendorDevices.emplace_back (std::move (platformName), platform, devices[0]);
    }

    return vendorDevices;
} // getDeviceFromAllPlatforms (cl_device_type device_type)

bool
DeviceProvider::setDefaultDeviceAndPlatformByVendorPriority (
    const std::vector <std::tuple <std::string, cl::Platform, cl::Device>>& vendorDevices,
    const std::vector <std::string>& vendorPriority
) {
    for (const auto& vendor : vendorPriority) {
        for (const auto&[platformName, platform, device] : vendorDevices) {
            if (platformName.find (vendor) != std::string::npos) {
                default_platform_ = platform;
                default_device_ = device;

                return true;
            }
        }
    }

    return false;
} // DeviceProvider::setDefaultDeviceAndPlatformByVendorPriority (
  //   const std::vector <std::tuple <std::string, cl::Platform, cl::Device>>& vendorDevices,
  //   const std::vector <std::string>& vendorPriority
  // )

void
DeviceProvider::initDefaultDeviceAndPlatform (const std::vector <std::string>& vendorPriority,
                                              cl_device_type device_type) {
    auto vendorDevices = getDeviceFromAllPlatforms (device_type);
    if (setDefaultDeviceAndPlatformByVendorPriority (vendorDevices, vendorPriority)) {
        return;
    }

    // Prepare error message
    std::stringstream err_msg;
    err_msg << "Failed to find the required devices according to the vendor priority list: "
            << vendorPriority << ". Found one device per platform: ";

    // Extract platform name
    IteratorExtractor iter_extractor_begin (vendorDevices.cbegin (),
        [] (const auto& tuple) {
            return std::get <std::string> (tuple); // Get platform name
        }
    );
    print_sep (err_msg, iter_extractor_begin, vendorDevices.cend ()) << ".";

    throw std::runtime_error (err_msg.str ());
} // DeviceProvider::initDefaultDeviceAndPlatform (const std::vector <std::string>& vendorPriority,
  //                                               cl_device_type device_type

DeviceProvider::DeviceProvider (cl_device_type device_type) : // Device type (CPU, GPU, ...)
    default_device_ (nullptr)
{
    initDefaultDeviceAndPlatform ({"NVIDIA", "AMD", "Intel"}, CL_DEVICE_TYPE_GPU);
} // DeviceProvider::DeviceProvider (cl_device_type device_type, std::string_view version)

cl::Platform
DeviceProvider::getDefaultPlatform () const {
    return default_platform_;
} // DeviceProvider::getDefaultPlatform () const
cl::Device DeviceProvider::getDefaultDevice () const {
    return default_device_;
} // DeviceProvider::getDefaultDevice () const

std::string
DeviceProvider::getDefaultPlatformName () const {
    return default_platform_.getInfo <CL_PLATFORM_NAME> ();
} // DeviceProvider::getDefaultPlatformName () const
std::string
DeviceProvider::getDefaultDeviceName () const {
    return default_device_.getInfo <CL_DEVICE_NAME> ();
} // DeviceProvider::getDefaultPlatformName () const

static unsigned
round_down_pow2 (unsigned num) // Number to be rounded
{
    return 1 << (8 * sizeof (num) - std::countl_zero (num) - 1);
} // round_down_pow2 (int num)

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
    cmd_queue_ (context_, CL_QUEUE_PROFILING_ENABLE),
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

    max_group_size_ = round_down_pow2 (max_group_size_ > max_possible_group_size ?
                                       max_possible_group_size :
                                       max_group_size_);

} // Sorter::Sorter (cl::Device device)

auto get_delta_time (cl::Event event) {
    event.wait ();
    return event.getProfilingInfo <CL_PROFILING_COMMAND_END> () -
           event.getProfilingInfo <CL_PROFILING_COMMAND_START> ();
}

template <>
Sorter::profiling_time_t
Sorter::sort <int> (int* input_data,       // Data to be sorted
                    std::size_t data_size, // The size of the data in the number of int
                    unsigned dir)              // Direction sort (1 -> /, 0 -> \)
{
    using data_type = cl_int4;

    // If array size less then minimum compared item (int4) * 2
    if (data_size < 2 * sizeof (data_type) / sizeof (input_data[0])) {
        std::sort (input_data, input_data + data_size, [dir = dir] (auto& lhs, auto& rhs) {
            return (lhs > rhs) ^ dir;
        });
        return 0;
    }

    data_size /= sizeof (data_type) / sizeof (input_data[0]);
    data_type* data = reinterpret_cast <data_type*> (input_data);

    unsigned l_buf_size = 2 * max_group_size_;

    cl::LocalSpaceArg local_buf { .size_ = sizeof (data_type) * l_buf_size };

    cl::Buffer buffer (context_, CL_MEM_READ_WRITE, data_size * sizeof (data_type));
    cmd_queue_.enqueueWriteBuffer (buffer, CL_FALSE, 0, sizeof (data_type) * data_size,
                                   input_data, nullptr, nullptr);

    profiling_time_t timeKernel = 0;

    if (data_size <= l_buf_size) {
        unsigned threadCount = data_size / 2;

        cl::NDRange global (threadCount);
        cl::NDRange local (threadCount);
        cl::EnqueueArgs args {cmd_queue_, global, local};

        cl::Event event = bitonic_sort_local_ (args, local_buf, buffer, data_size, dir);
        cl::copy (cmd_queue_, buffer, data, data + data_size);

        timeKernel += get_delta_time (event);
    } else {
        unsigned threadCount = l_buf_size / 2;
        unsigned blockCount = data_size / l_buf_size;

        cl::NDRange global (blockCount * threadCount);
        cl::NDRange local (threadCount);
        cl::EnqueueArgs args {cmd_queue_, global, local};

        std::vector <cl::Event> event_list;

        event_list.push_back (bitonic_sort_full_local_ (args, local_buf, buffer));
        for (unsigned size = 2 * l_buf_size; size <= data_size; size <<= 1)
        for (unsigned stride = size / 2; stride > 0; stride >>= 1)
            if (stride >= l_buf_size) {
                event_list.push_back (
                bitonic_merge_global_ (args, buffer, data_size, size, stride, dir));
            } else {
                event_list.push_back (
                bitonic_merge_local_ (args, local_buf, buffer, data_size, size, dir));
                break;
            }

        cl::copy (cmd_queue_, buffer, data, data + data_size);

        for (const auto& event : event_list) {
            timeKernel += get_delta_time (event);
        }
    }

    return timeKernel;
} // Sorter::sort <int> (int* input_data, std::size_t data_size, unsigned dir)

} // namespace hidra