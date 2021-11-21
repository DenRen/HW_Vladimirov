#include "cppl.hpp"

#include <fstream>
#include <string>
#include <iostream>

namespace hidra {

DeviceProvider::DeviceProvider (cl_device_type device_type,
                                std::string_view version) :
    defualt_device_ (nullptr)
{
    std::vector <cl::Platform> platforms;
    cl::Platform::get (&platforms);

    if (platforms.size () == 0) {
        throw cl::Error (0, "Platforms not found");
    }

    for (const auto& platform : platforms) {
        auto platform_version = platform.getInfo <CL_PLATFORM_VERSION> ();

        if (platform_version.find (version) != decltype (platform_version)::npos) {
            std::vector <cl::Device> devices;

            platform.getDevices (device_type, &devices);
            if (devices.size () != 0) {
                default_platform_ = platform;
                defualt_device_ = devices[0];
            }
        }
    }

    if (defualt_device_ () == nullptr) {
        throw cl::Error (0, "Device not dounf in platform!");
    }
}

cl::Platform
DeviceProvider::getDefaultPlatform () const {
    return default_platform_;
}
cl::Device DeviceProvider::getDefautDevice () const {
    return defualt_device_;
}

std::string
DeviceProvider::getDefaultPlatformName () const {
    return default_platform_.getInfo <CL_PLATFORM_NAME> ();
}
std::string
DeviceProvider::getDefaultDeviceName () const {
    return defualt_device_.getInfo <CL_DEVICE_NAME> ();
}

static std::string
readSource (std::string fileName) {
    using stream = std::istreambuf_iterator <std::string::traits_type::char_type>;

    std::ifstream file (fileName);
    return std::string (stream (file), stream ());
}

Adder::Adder (cl::Device device) :
    device_ (device),
    context_ (device_),
    cmd_queue_ (context_),
    program_ (context_, readSource ("adder.cl"), true)
{}

template <>
std::string_view Adder::getFuncName_VectAddInArg <int> () {
    return "vector_add_in_arg_i32";
}

template <>
std::string_view
Adder::getFuncName_VectAddInArg <long> () {
    return "vector_add_in_arg_i64";
}

template <>
std::string_view
Adder::getFuncName_VectAddInArg <unsigned> () {
    return "vector_add_in_arg_u32";
}

template <>
std::string_view Adder::getFuncName_VectAddInArg <unsigned long> () {
    return "vector_add_in_arg_u64";
}

template <>
std::string_view Adder::getFuncName_VectAdd <int> () {
    return "vector_add_i32";
}

template <>
std::string_view Adder::getFuncName_VectAdd <long> () {
    return "vector_add_i64";
}

template <>
std::string_view Adder::getFuncName_VectAdd <unsigned> () {
    return "vector_add_u32";
}

template <>
std::string_view Adder::getFuncName_VectAdd <unsigned long> () {
    return "vector_add_u64";
}

}