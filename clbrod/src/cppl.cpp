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
    
    std::vector <std::string> vendorNames;
    std::vector <std::tuple <cl::Platform, cl::Device>> vendorDevice;
    vendorNames.reserve (platforms.size ());

    for (const auto& platform : platforms) {
        auto platform_version = platform.getInfo <CL_PLATFORM_VERSION> ();
        if (platform_version.find (version) != decltype (platform_version)::npos) {  
            auto platformName = platform.getInfo <CL_PLATFORM_NAME> ();

            std::vector <cl::Device> devices;
            platform.getDevices (device_type, &devices);
            if (devices.size () == 0) {
                continue;
            }

            vendorNames.emplace_back (std::move (platformName));
            vendorDevice.push_back ({platform, devices[0]});
        }
    }

    const std::vector <std::string> vendorPriority {"NVIDIA", "AMD", "Intel"};
    bool platformFounded = false;

    for (const auto& vendor : vendorPriority) {
        std::size_t index = -1;
        for (const auto& vendorName : vendorNames) {
            ++index;
            if (vendorName.find (vendor) != std::string::npos) {
                default_platform_ = std::get <cl::Platform> (vendorDevice[index]);
                defualt_device_ = std::get <cl::Device> (vendorDevice[index]);

                platformFounded = true;
                break;
            }
        }
        
        if (platformFounded) {
            break;
        }
    }

    if (!platformFounded) {
        std::stringstream err_msg ("Device with ");
        err_msg << version;
        err_msg << " from ";
        err_msg << vendorPriority;
        err_msg << " not found in platform!";

        throw std::runtime_error (err_msg.str ());
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

cl::Program
buildProgram (cl::Context context,           // The context in which the program will be built
              std::string name_kernel_func)  // Name kernel function
{
    cl::Program program (context, readSource (name_kernel_func));
    try {
        std::string_view options = "-cl-unsafe-math-optimizations \
                                    -cl-fast-relaxed-math\
                                    -cl-mad-enable";
        program.build (/*options.data ()*/);
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