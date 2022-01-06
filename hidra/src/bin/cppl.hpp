#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_ENABLE_EXCEPTIONS

#include <CL/opencl.hpp>
#include <iostream>

namespace hidra {

class DeviceProvider {
    std::string opencl_version_;

    cl::Platform default_platform_;
    cl::Device default_device_;

public:
    DeviceProvider (cl_device_type device_type = CL_DEVICE_TYPE_GPU);

    cl::Platform getDefaultPlatform () const;
    cl::Device getDefaultDevice () const;

    std::string getDefaultPlatformName () const;
    std::string getDefaultDeviceName () const;

    static std::string dumpAll ();

private:
    void initDefaultDeviceAndPlatform (const std::vector <std::string>& vendorPriority,
                                       cl_device_type device_type); // Device type (CPU, GPU, ...)
    bool setDefaultDeviceAndPlatformByVendorPriority (
        const std::vector <std::tuple <std::string, cl::Platform, cl::Device>>& vendorDevices,
        const std::vector <std::string>& vendorPriority
    );
}; // class DeviceProvider

class Sorter {
    cl::Device device_;
    cl::Context context_;
    cl::CommandQueue cmd_queue_;

    cl::Program program_;

    cl::KernelFunctor <cl::LocalSpaceArg, cl::Buffer, cl_uint, cl_uint>
        bitonic_sort_local_;
    cl::KernelFunctor <cl::LocalSpaceArg, cl::Buffer>
        bitonic_sort_full_local_;
    cl::KernelFunctor <cl::Buffer, cl_uint, cl_uint, cl_uint, cl_uint>
        bitonic_merge_global_;
    cl::KernelFunctor <cl::LocalSpaceArg, cl::Buffer, cl_uint, cl_uint, cl_uint>
        bitonic_merge_local_;

    unsigned max_group_size_;

    using profiling_time_t = decltype (cl::Event ().getProfilingInfo <CL_PROFILING_COMMAND_START> ());

public:
    Sorter (cl::Device device);

    profiling_time_t
    sort (std::vector <int>& vec, unsigned dir = 1) {
        return sort (vec.data (), vec.size (), dir);
    }

    profiling_time_t
    sort (int* data, size_t size, unsigned dir = 1);

}; // class Sorter

} // namespace hidra
