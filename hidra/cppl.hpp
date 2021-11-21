#pragma once

#include "CL/cl.h"

namespace hidra {
    // Reference Handler ------------------------------------------------------
    template <typename T>
    struct ReferenceHandler {};

    template <>
    struct ReferenceHandler <cl_device_id> {
        static cl_int retain  (cl_device_id device) { return clRetainDevice (device); }
        static cl_int release (cl_device_id device) { return clReleaseDevice (device); }
    };

    template <>
    struct ReferenceHandler <cl_context> {
        static cl_int retain  (cl_context context) { return clRetainContext (context); }
        static cl_int release (cl_context context) { return clReleaseContext (context); }
    };

    template <>
    struct ReferenceHandler <cl_command_queue> {
        static cl_int retain  (cl_command_queue cmd_queue) { return clRetainCommandQueue (cmd_queue); }
        static cl_int release (cl_command_queue cmd_queue) { return clReleaseCommandQueue (cmd_queue); }
    };

    template <>
    struct ReferenceHandler <cl_program> {
        static cl_int retain  (cl_program program) { return clRetainProgram (program); }
        static cl_int release (cl_program program) { return clReleaseProgram (program); }
    };

    template <>
    struct ReferenceHandler <cl_kernel> {
        static cl_int retain  (cl_kernel kernel) { return clRetainKernel (kernel); }
        static cl_int release (cl_kernel kernel) { return clReleaseKernel (kernel); }
    };

    template <>
    struct ReferenceHandler <cl_mem> {
        static cl_int retain  (cl_mem mem) { return clRetainMemObject (mem); }
        static cl_int release (cl_mem mem) { return clReleaseMemObject (mem); }
    };

    // Detail -----------------------------------------------------------------
    namespace detail {
        template <typename CL_TYPE>
        class Wrapper {
        protected:
            CL_TYPE object_;
        
        public:
            Wrapper (CL_TYPE& object) :
                object_ (object)
            {}

            ~Wrapper () {
                release ();
            }

            cl_int release () const {
                if (object_ == nullptr) {
                    return CL_SUCCESS;
                }

                return ReferenceHandler <CL_TYPE>::release (object_);
            }

            const CL_TYPE& operator () () const {
                return object_;
            }
            CL_TYPE& operator () () {
                return object_;
            }

            CL_TYPE get () const {
                return object_;
            }
        };
    }

    // Device -----------------------------------------------------------------
    class Device : public detail::Wrapper <cl_device_id> {

    };
}