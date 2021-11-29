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

class Adder {
    cl::Device device_;
    cl::Context context_;
    cl::CommandQueue cmd_queue_;

    cl::Program program_;

public:
    Adder (cl::Device device);

    template <typename T>
    void
    vect_add (T* A, T* B, T* res, size_t size) {
        if (res == A) {
            vect_add_in_arg (B, A, size);
        } else if (res == B) {
            vect_add_in_arg (A, B, size);
        } else {
            _vect_add (A, B, res, size);
        }
    }

private:
    template <typename T>
    std::string_view getFuncName_VectAddInArg ();

    // Result is write in B
    template <typename T>
    void
    vect_add_in_arg (T* A, T* B, size_t size) {
        const size_t buf_size = size * sizeof (T);

        cl::Buffer buffer_A (context_, CL_MEM_READ_ONLY, buf_size);
        cl::Buffer buffer_B (context_, CL_MEM_READ_WRITE, buf_size);

        cl::copy (cmd_queue_, A, A + size, buffer_A);
        cl::copy (cmd_queue_, B, B + size, buffer_B);

        cl::NDRange global (size);
        cl::NDRange local (64);
        cl::EnqueueArgs enc_args {cmd_queue_, global, local};

        std::string_view name_function = getFuncName_VectAddInArg <T> ();
        cl::KernelFunctor <cl::Buffer, cl::Buffer> add_vectors (program_, name_function.data ());

        add_vectors (enc_args, buffer_A, buffer_B);

        cl::copy (cmd_queue_, buffer_B, B, B + size);
    }

    template <typename T>
    std::string_view getFuncName_VectAdd ();

    template <typename T>
    void
    _vect_add (T* A, T* B, T* res, size_t size) {
        const size_t buf_size = size * sizeof (T);

        cl::Buffer buffer_A (context_, CL_MEM_READ_ONLY, buf_size);
        cl::Buffer buffer_B (context_, CL_MEM_READ_ONLY, buf_size);
        cl::Buffer buffer_res (context_, CL_MEM_WRITE_ONLY, buf_size);

        cl::copy (cmd_queue_, A, A + size, buffer_A);
        cl::copy (cmd_queue_, B, B + size, buffer_B);

        cl::NDRange global (size);
        cl::NDRange local (64);
        cl::EnqueueArgs enc_args {cmd_queue_, global, local};

        std::string_view name_function = getFuncName_VectAdd <T> ();
        cl::KernelFunctor <cl::Buffer, cl::Buffer, cl::Buffer> add_vectors (program_, name_function.data ());

        add_vectors (enc_args, buffer_A, buffer_B, buffer_res);

        cl::copy (cmd_queue_, buffer_res, res, res + size);
    }
}; // class Adder

class Sorter {
    cl::Device device_;
    cl::Context context_;
    cl::CommandQueue cmd_queue_;

    cl::Program program_;
    cl::KernelFunctor <cl::Buffer, cl::LocalSpaceArg, cl_int> sort_i4_;
    cl::KernelFunctor <cl::Buffer, cl::LocalSpaceArg, cl_int, cl_uint> cmptr_i16_;

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

    // Invoke half sorter
    template <typename T>
    void
    half_sort (T* data, size_t size) {
        const size_t buffer_size = size * sizeof (T);

        cl::Buffer buffer (context_, CL_MEM_READ_WRITE, buffer_size);
        cl::copy (cmd_queue_, data, data + size, buffer);

        cl::KernelFunctor <cl::Buffer, unsigned long> sort (program_, "_half_filter");

        cl::NDRange global (size / 2);
        cl::NDRange local (std::max (size / 2, (size_t) 1));
        cl::EnqueueArgs enc_args {cmd_queue_, global, local};

        sort (enc_args, buffer, size / 2);

        cl::copy (cmd_queue_, buffer, data, data + size);
    }


    // Invoke unifying network
    template <typename T>
    void
    unifying_network (T* data, size_t size) {
        const size_t buffer_size = size * sizeof (T);

        cl::Buffer buffer (context_, CL_MEM_READ_WRITE, buffer_size);
        cl::copy (cmd_queue_, data, data + size, buffer);

        cl::KernelFunctor <cl::Buffer, unsigned long> sort (program_, "_unifying_network");

        cl::NDRange global (size / 2);
        cl::NDRange local (std::max (size / 2, (size_t) 1));
        cl::EnqueueArgs enc_args {cmd_queue_, global, local};

        sort (enc_args, buffer, size / 2);

        cl::copy (cmd_queue_, buffer, data, data + size);
    }

private:
    void _sort_i1024 (int* data);
};
}