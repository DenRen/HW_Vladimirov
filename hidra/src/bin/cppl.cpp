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

ClAccelerator::ClAccelerator (const cl::Device& device,
                              const std::string& source,
                              cl::QueueProperties cmd_queue_prop) :
    device_ (device),
    context_ (device),
    cmd_queue_ (context_, cmd_queue_prop),
    program_ (hidra::buildProgram (context_, source))
{}

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

// Extractor iterator
template <typename IteratorT, typename ExtractorT>
class IteratorExtractor {
    IteratorT current_;
    ExtractorT extractor_;

public:
    IteratorExtractor (IteratorT begin, ExtractorT extractor) :
        current_ (begin),
        extractor_ (extractor)
    {}

    IteratorExtractor&
    operator ++ () {
        ++current_;
        return *this;
    }

    IteratorExtractor
    operator ++ (int) {
        return IteratorExtractor (current_++, extractor_);
    }

    auto
    operator * () const {
        return extractor_ (*current_);
    }

    bool
    operator != (const IteratorExtractor& rhs) const {
        return current_ != rhs.current_;
    }

    bool
    operator == (const IteratorExtractor& rhs) const {
        return !(*this != rhs);
    }

    bool
    operator != (const IteratorT& rhs) const {
        return current_ != rhs;
    }

    bool
    operator == (const IteratorT& rhs) const {
        return !(*this != rhs);
    }
};

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

cl::Program
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

Sorter::profiling_time_t
Sorter::sort (int* input_data,      // Data to be sorted
             std::size_t data_size, // The size of the data in the number of int
             unsigned dir)          // Direction sort (1 -> /, 0 -> \)
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

Finder::Finder (const cl::Device& device) :
    ClAccelerator (device, "config/kernels/finder.cl")
{}

std::vector <int>
Finder::numberRepeats (const std::string& haystack,
                       const std::vector <std::string>& needles)
{
    return {};
}


const char *getErrorString (cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}

void printError (cl::Error& error) {
    std::cout << error.what () << ", error code: " << error.err ()
              << " => " << hidra::getErrorString (error.err ()) << std::endl;
}

} // namespace hidra