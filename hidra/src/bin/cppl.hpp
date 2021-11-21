#pragma once

#define __CL_ENABLE_EXCEPTIONS
#include "CL/cl2.hpp"

namespace hidra {

class DeviceProvider {
    std::string opencl_version_;

    cl::Platform default_platform_;
    cl::Device defualt_device_;

public:
    DeviceProvider (cl_device_type device_type = CL_DEVICE_TYPE_GPU,
                    std::string_view version = "OpenCL 3.");

    cl::Platform getDefaultPlatform () const;
    cl::Device getDefautDevice () const;

    std::string getDefaultPlatformName () const;
    std::string getDefaultDeviceName () const;

    static std::string dumpAll (); // todo
};

class Adder {
    cl::Device device_;
    cl::Context context_;
    cl::CommandQueue cmd_queue_;

    cl::Program program_;

public:
    Adder (cl::Device device);

    template <typename T>
    void vect_add (T* A, T* B, T* res, size_t size) {
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
    void vect_add_in_arg (T* A, T* B, size_t size) {
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
    void _vect_add (T* A, T* B, T* res, size_t size) {
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
}; // Adder
}