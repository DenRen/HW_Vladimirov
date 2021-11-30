#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#include "CL/cl2.hpp"
#include <iostream>

namespace hidra {

class DeviceProvider {
    std::string opencl_version_;

    cl::Platform default_platform_;
    cl::Device defualt_device_;

public:
    DeviceProvider (cl_device_type device_type = CL_DEVICE_TYPE_GPU,
                    std::string_view version = "OpenCL 3.");

    cl::Platform getDefaultPlatform () const;
    cl::Device getDefaultDevice () const;

    std::string getDefaultPlatformName () const;
    std::string getDefaultDeviceName () const;

    static std::string dumpAll ();
}; // class DeviceProvider

class Sorter {
    cl::Device device_;
    cl::Context context_;
    cl::CommandQueue cmd_queue_;

    cl::Program program_;
    cl::KernelFunctor <cl::Buffer, cl::LocalSpaceArg, cl_int> sort_i4_;
    cl::KernelFunctor <cl::Buffer, cl::LocalSpaceArg, cl_uint> big_sort_i4_;

    uint32_t max_group_size_;

public:
    Sorter (cl::Device device);

    template <typename T>
    void
    vect_sort (std::vector <T>& vec) {
        vect_sort (vec.data (), vec.size ());
    }

    template <typename T>
    void
    vect_sort (T* data, size_t size);

}; // class Sorter

} // namespace hidra