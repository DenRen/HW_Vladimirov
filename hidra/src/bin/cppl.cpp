#include "cppl.hpp"

#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include <boost/type_index.hpp>

using boost::typeindex::type_id_with_cvr;

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
        std::string err_msg ("Device with version");
        err_msg += version;
        err_msg += "not found in platform!";

        throw cl::Error (0, err_msg.c_str ());
    }
}

static void
print_words (std::stringstream& stream, std::string words,
             int num_tabs = 1, const char sep = ' ') {
    std::size_t begin_pos = 0;

    for (std::size_t space_pos = words.find (sep);
         space_pos != std::string::npos;
         space_pos = words.find (sep, begin_pos))
    {
        size_t len = space_pos - begin_pos;
        for (int i = 0; i < num_tabs; ++i) {
            stream << "\t";
        }

        stream << words.substr (begin_pos, len) << std::endl;
        begin_pos = space_pos + 1;
    }
}

static std::stringstream&
getAllPlatformInfo (std::stringstream& stream, const cl::Platform& platform) {
    stream
        << "Profile: " << platform.getInfo <CL_PLATFORM_PROFILE> () << std::endl
        << "Version: " << platform.getInfo <CL_PLATFORM_VERSION> () << std::endl
        << "Name: "    << platform.getInfo <CL_PLATFORM_NAME>    () << std::endl
        << "Vecndor: " << platform.getInfo <CL_PLATFORM_VENDOR>  () << std::endl;

    std::string extensions = platform.getInfo <CL_PLATFORM_EXTENSIONS> ();
    stream << "Ectentions: " << std::endl;

    print_words (stream, extensions);

    return stream;
}

template <typename Traits, typename CharT>
std::basic_ostream <CharT, Traits>&
deviceTypeToStringstream (std::basic_ostream <CharT, Traits>& stream, cl_device_type type) {
    switch (type) {
    case CL_DEVICE_TYPE_DEFAULT:
        return stream << "Default";
    case CL_DEVICE_TYPE_CPU:
        return stream << "CPU";
    case CL_DEVICE_TYPE_GPU:
        return stream << "GPU";
    case CL_DEVICE_TYPE_ACCELERATOR:
        return stream << "ACCELERATOR";
    case CL_DEVICE_TYPE_CUSTOM:
        return stream << "CUSTOM";
    case CL_DEVICE_TYPE_ALL:
        return stream << "ALL";
    default:
        return stream << "Undefined";
    };

    return stream;
} // deviceTypeToStringstream

template <typename T, typename Traits, typename CharT>
std::basic_ostream <CharT, Traits>&
operator << (std::basic_ostream <CharT, Traits>& stream, const std::vector <T>& vec) {
    const std::size_t size = vec.size ();
    if (size != 0) {
        for (std::size_t i = 0; i + 1 < size; ++i) {
            stream << vec[i] << " ";
        }
        stream << vec[size - 1];
    }

    return stream;
}

template <typename CharT, typename Traits>
std::basic_ostream <CharT, Traits>&
tab (std::basic_ostream <CharT, Traits>& os) {
    return os.put (os.widen ('\t'));
}

#undef PRINT
#define PRINT(str, info_id) \
    << tab << str ": " << device.getInfo <info_id> () << std::endl

static std::stringstream&
getAllDeviceInfo (std::stringstream& stream, const cl::Device& device) {
    stream << tab << "Device type: ";
    deviceTypeToStringstream (stream, device.getInfo <CL_DEVICE_TYPE> ()) << std::endl;

    stream
        PRINT ("Vendor Id",                 CL_DEVICE_VENDOR_ID)
        PRINT ("Max compute units",         CL_DEVICE_MAX_COMPUTE_UNITS)
        PRINT ("Max work item dimensions",  CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS)
        PRINT ("Max work group size",       CL_DEVICE_MAX_WORK_GROUP_SIZE)
        PRINT ("Max work item sizes",       CL_DEVICE_MAX_WORK_ITEM_SIZES)
        << std::endl
        PRINT ("Prereferd vector width char",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR)
        PRINT ("Prereferd vector width short",  CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT)
        PRINT ("Prereferd vector width int" ,   CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT)
        PRINT ("Prereferd vector width long",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG)
        PRINT ("Prereferd vector width float",  CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT)
        PRINT ("Prereferd vector width double", CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE)
        << std::endl
        PRINT ("Max clock frequency",    CL_DEVICE_MAX_CLOCK_FREQUENCY)
        PRINT ("Address bits",           CL_DEVICE_ADDRESS_BITS)
        PRINT ("Max read image args",    CL_DEVICE_MAX_READ_IMAGE_ARGS)
        PRINT ("Max write image args"  , CL_DEVICE_MAX_WRITE_IMAGE_ARGS)
        PRINT ("Max memory alloc size" , CL_DEVICE_MAX_MEM_ALLOC_SIZE)
        << std::endl
        PRINT ("Image support"      , CL_DEVICE_IMAGE_SUPPORT)
        PRINT ("Image2D max width"  , CL_DEVICE_IMAGE2D_MAX_WIDTH)
        PRINT ("Image2D max height" , CL_DEVICE_IMAGE2D_MAX_HEIGHT)
        PRINT ("Image3D max width"  , CL_DEVICE_IMAGE3D_MAX_WIDTH)
        PRINT ("Image3D max height" , CL_DEVICE_IMAGE3D_MAX_HEIGHT)
        PRINT ("Image3D max depth"  , CL_DEVICE_IMAGE3D_MAX_DEPTH)
        << std::endl
        PRINT ("Max parameter size",        CL_DEVICE_MAX_PARAMETER_SIZE)
        PRINT ("Max samplers",              CL_DEVICE_MAX_SAMPLERS)
        PRINT ("Memory base addr align",    CL_DEVICE_MEM_BASE_ADDR_ALIGN)
        PRINT ("Min data type align size",  CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE)
        PRINT ("Single FP config",          CL_DEVICE_SINGLE_FP_CONFIG) // todo
        << std::endl
        PRINT ("Global memory cache type",      CL_DEVICE_GLOBAL_MEM_CACHE_TYPE)
        PRINT ("Global memory cache line size", CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE)
        PRINT ("Global memory size",            CL_DEVICE_GLOBAL_MEM_SIZE)
        PRINT ("Max constant buffer size",      CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE)
        PRINT ("Max constant args",             CL_DEVICE_MAX_CONSTANT_ARGS)
        PRINT ("Local memory type",             CL_DEVICE_LOCAL_MEM_TYPE)
        PRINT ("Lcoal memory size",             CL_DEVICE_LOCAL_MEM_SIZE)
        << std::endl
        PRINT ("Error correction support",   CL_DEVICE_ERROR_CORRECTION_SUPPORT)
        PRINT ("Profiling timer resolution", CL_DEVICE_PROFILING_TIMER_RESOLUTION)
        PRINT ("Little endian",              CL_DEVICE_ENDIAN_LITTLE)
        PRINT ("Available",                  CL_DEVICE_AVAILABLE)
        << std::endl
        PRINT ("Compiler available",       CL_DEVICE_COMPILER_AVAILABLE)
        PRINT ("Execution capabilities",   CL_DEVICE_EXECUTION_CAPABILITIES)
        PRINT ("Queue properties",         CL_DEVICE_QUEUE_PROPERTIES)
        PRINT ("Queue on host properties", CL_DEVICE_QUEUE_ON_HOST_PROPERTIES)
        << std::endl
        PRINT ("Name",           CL_DEVICE_NAME)
        PRINT ("Vendor",         CL_DEVICE_VENDOR)
        PRINT ("Driver version", CL_DRIVER_VERSION)
        PRINT ("Profile",        CL_DEVICE_PROFILE)
        PRINT ("Platform",       CL_DEVICE_PLATFORM)
        << std::endl;

        std::string extensions = device.getInfo <CL_DEVICE_EXTENSIONS> ();
        stream << tab << "Ectentions: " << std::endl;

        print_words (stream, extensions, 2);

        return stream;
} // getAllDeviceInfo (std::stringstream& stream, const cl::Device& device)

#undef PRINT

std::string
DeviceProvider::dumpAll () {
    std::stringstream stream;

    std::vector <cl::Platform> platforms;
    cl::Platform::get (&platforms);

    if (platforms.size () == 0) {
        return "Platforms not found";
    }

    for (const auto& platform : platforms) {
        getAllPlatformInfo (stream, platform);

        std::vector <cl::Device> devices;
        platform.getDevices (CL_DEVICE_TYPE_ALL, &devices);
        for (const auto& device : devices) {
            stream << std::endl << "Device " << device.getInfo <CL_DEVICE_NAME> ()
                   << ":" << std::endl;
            getAllDeviceInfo (stream, device);
        }

        return stream.str ();
    }
}

cl::Platform
DeviceProvider::getDefaultPlatform () const {
    return default_platform_;
}
cl::Device DeviceProvider::getDefaultDevice () const {
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
    program_ (context_, readSource ("kernels/adder.cl"), true)
{}

template <>
std::string_view
Adder::getFuncName_VectAddInArg <int> () {
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
std::string_view
Adder::getFuncName_VectAddInArg <unsigned long> () {
    return "vector_add_in_arg_u64";
}

template <>
std::string_view
Adder::getFuncName_VectAdd <int> () {
    return "vector_add_i32";
}

template <>
std::string_view
Adder::getFuncName_VectAdd <long> () {
    return "vector_add_i64";
}

template <>
std::string_view
Adder::getFuncName_VectAdd <unsigned> () {
    return "vector_add_u32";
}

template <>
std::string_view
Adder::getFuncName_VectAdd <unsigned long> () {
    return "vector_add_u64";
}

Sorter::Sorter (cl::Device device) :
    device_ (device),
    context_ (device_),
    cmd_queue_ (context_),
    program_ (context_, readSource ("kernels/sorter_v5.cl")),
    max_group_size_ (device.getInfo <CL_DEVICE_MAX_WORK_GROUP_SIZE> ())
{
    try {
        program_.build ();
    } catch (cl::Error& exc) {
        cl_int buildError = CL_SUCCESS;

        auto buildInfo = program_.getBuildInfo <CL_PROGRAM_BUILD_LOG> (&buildError);

        for (const auto& pair : buildInfo) {
            auto device = pair.first.getInfo <CL_DEVICE_NAME> ();
            auto build_error_msg = pair.second;
            std::cerr << "Device: " << device << std::endl;
            std::cerr << "Build msg: " << build_error_msg << std::endl;
        }

        throw;
    }
}

void Sorter::_sort_i1024 (int* data) {
        const size_t size = 1024;
        const size_t size_cell = 8;

        const size_t buffer_size = size * sizeof (int);
        cl::Buffer buffer (context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR ,
                           buffer_size, data);

        const uint32_t size_item_data = size_cell * sizeof (int);
        const uint32_t number_work_items = buffer_size / size_item_data;
        const uint32_t number_work_group = number_work_items / max_group_size_;

        cl::KernelFunctor <cl::Buffer, cl::LocalSpaceArg>
            kernelFucntor (program_, "vector_sort_i4");

        cl::NDRange global (max_group_size_);
        cl::NDRange local (max_group_size_);
        cl::EnqueueArgs args {cmd_queue_, global, local};

        cl::LocalSpaceArg local_buf {
            .size_ = max_group_size_ * size_item_data
        };
        kernelFucntor (args, buffer, local_buf);
        cl::copy (cmd_queue_, buffer, data, data + size);
    }

static int
round_down_pow2 (int n) {
    int i = 0;
    for (; n != 0; ++i) {
        n >>= 1;
    }

    return 1 << (i - 1);
}

template <>
void
Sorter::vect_sort <int> (int* data, size_t size) {
    int _n = device_.getInfo <CL_DEVICE_LOCAL_MEM_SIZE> () /
             (2 * sizeof (int) * max_group_size_);
    _n = round_down_pow2 (_n);

    std::string name_func = std::string ("vector_sort_i") + std::to_string (_n);
    size_t size_block = 2 * _n * max_group_size_;

    if (size % size_block != 0) {
        std::cerr << "size: " << size << ", size_block: " << size_block << std::endl;
        throw std::runtime_error ("size %% size_block != 0");
    }

    cl::KernelFunctor <cl::Buffer, cl::LocalSpaceArg>
        kernelFucntor (program_, name_func);
    
    cl::NDRange global (max_group_size_);
    cl::NDRange local (max_group_size_);
    cl::EnqueueArgs args {cmd_queue_, global, local};

    cl::LocalSpaceArg local_buf {
        .size_ = size_block * sizeof (int)
    };

    const int num_blocks = size / size_block;

    int* cur_data = data;
    for (int group_id = 0; group_id < num_blocks; ++group_id) {
        const size_t buffer_size = size_block * sizeof (int);
        cl::Buffer buffer (context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR ,
                           buffer_size, cur_data);

        kernelFucntor (args, buffer, local_buf);
        cl::copy (cmd_queue_, buffer, cur_data, cur_data + size_block);

        cur_data += size_block;
    }
}

} // namespace hidra