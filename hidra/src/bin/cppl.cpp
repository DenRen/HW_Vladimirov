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
print_words (std::stringstream& stream, // The output stream
            std::string words,          // Words separated sep
             int num_tabs = 1,          // Number of indents
             const char sep = ' ')      // Separator symbol
{
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
getAllPlatformInfo (std::stringstream& stream,    // The output stream
                    const cl::Platform& platform) // Platform under test
{
    stream
        << "Profile: " << platform.getInfo <CL_PLATFORM_PROFILE> () << std::endl
        << "Version: " << platform.getInfo <CL_PLATFORM_VERSION> () << std::endl
        << "Name: "    << platform.getInfo <CL_PLATFORM_NAME>    () << std::endl
        << "Vecndor: " << platform.getInfo <CL_PLATFORM_VENDOR>  () << std::endl;

    std::string extensions = platform.getInfo <CL_PLATFORM_EXTENSIONS> ();
    stream << "Ectentions: " << std::endl;

    print_words (stream, extensions);

    return stream;
} // getAllPlatformInfo (std::stringstream& stream, const cl::Platform& platform)

template <typename Traits, typename CharT>
std::basic_ostream <CharT, Traits>&
deviceTypeToStringstream (std::basic_ostream <CharT, Traits>& stream, // The output stream
                          cl_device_type type)                        // Device type
{
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
} // deviceTypeToStringstream (std::basic_ostream <CharT, Traits>& stream, cl_device_type type)

#undef PRINT
#define PRINT(str, info_id) \
    << tab << str ": " << device.getInfo <info_id> () << std::endl

static std::stringstream&
getAllDeviceInfo (std::stringstream& stream, // The stream where the device info string will be written
                  const cl::Device& device)  // Device under test
{
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
} // DeviceProvider::dumpAll ()

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

static std::string
readSource (std::string fileName) // The name of the file from which everything will be read
{
    using stream = std::istreambuf_iterator <std::string::traits_type::char_type>;

    std::ifstream file (fileName);
    return std::string (stream (file), stream ());
} // readSource (std::string fileName)

static int
round_down_pow2 (int n) // Number to be rounded
{
    int i = 0;
    for (; n != 0; ++i) {
        n >>= 1;
    }

    return 1 << (i - 1);
} // round_down_pow2 (int n)

static cl::Program
buildProgram (cl::Context context,           // The context in which the program will be built
              std::string name_kernel_func)  // Name kernel function
{
    cl::Program program (context, readSource (name_kernel_func));
    try {
        program.build ();
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
    cmd_queue_ (context_),
    program_ (buildProgram (context_, "kernels/sorter_v6.cl")),
    sort_i4_ (program_, "vector_sort_i4"),
    big_sort_i4_ (program_, "big_vector_sort_i4"),
    max_group_size_ (device.getInfo <CL_DEVICE_MAX_WORK_GROUP_SIZE> ())
{
    const std::size_t local_size = device.getInfo <CL_DEVICE_LOCAL_MEM_SIZE> ();
    const std::size_t max_possible_group_size = local_size / (4 * 2 * sizeof (int));
    if (max_group_size_ > max_possible_group_size) {
        max_group_size_ = round_down_pow2 (max_possible_group_size);
    }

    max_group_size_ = round_down_pow2 (max_group_size_);
} // Sorter::Sorter (cl::Device device)

template <>
void
Sorter::vect_sort <int> (int* data,   // Data to be sorted
                         size_t size) // The size of the data in the number of int
{
    size_t size_block = 2 * 4 * sizeof (int) * max_group_size_;

    std::size_t num_int_on_work_item = 8;

    // Prepeare size and add_size
    std::size_t add_size = 0;
    if (size < num_int_on_work_item) {
        add_size = num_int_on_work_item - size;
    } else {
        unsigned n = std::log2 (2 * size / num_int_on_work_item);
        while ((1 << n) < size) {
            add_size = (1 << ++n);
        }
        add_size -= size;
    }

    std::size_t full_size = size + add_size;

    // Number necessary work items
    std::size_t num_items = full_size / num_int_on_work_item;

    if (num_items <= max_group_size_) {
        cl::NDRange global (num_items);
        cl::NDRange local (num_items);
        cl::EnqueueArgs args {cmd_queue_, global, local};

        cl::LocalSpaceArg local_buf {
            .size_ = sizeof (int) * full_size
        };

        cl::Buffer buffer (context_, CL_MEM_READ_WRITE, full_size * sizeof (int));
        cl::copy (cmd_queue_, data, data + size, buffer);

        if (add_size != 0) {
            std::vector <int> poison (add_size, INT32_MAX);
            cmd_queue_.enqueueWriteBuffer (buffer, true, sizeof (int) * size,
                                           sizeof (int) * add_size, poison.data ());
        }

        sort_i4_ (args, buffer, local_buf, 0);
        cl::copy (cmd_queue_, buffer, data, data + size);
    } else {
        cl::NDRange global (max_group_size_);
        cl::NDRange local (max_group_size_);
        cl::EnqueueArgs args {cmd_queue_, global, local};

        cl::LocalSpaceArg local_buf {
            .size_ = 2 * 4 * sizeof (int) * max_group_size_
        };

        cl::Buffer buffer (context_, CL_MEM_READ_WRITE, full_size * sizeof (int));
        cl::copy (cmd_queue_, data, data + size, buffer);

        std::vector <int> poison (add_size, INT32_MAX);
        if (add_size != 0) {
            cmd_queue_.enqueueWriteBuffer (buffer, true, sizeof (int) * size,
                                           sizeof (int) * add_size, poison.data ());
        }

        const std::size_t number_blocks = full_size / (4 * max_group_size_);
        big_sort_i4_ (args, buffer, local_buf, number_blocks);

        cl::copy (cmd_queue_, buffer, data, data + size);
    }
} // Sorter::vect_sort <int> (int* data, size_t size)

void
testSpeed () {
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    hidra::Sorter sorter (device);

    std::random_device rd;
    std::mt19937 mersenne (rd ());

    const size_t min_size_arr = 8 * 1 << 4;
    const size_t max_size_arr = 8 * 1 << 20;
    const size_t repeat = 10;

    for (std::size_t size_arr = min_size_arr; size_arr <= max_size_arr; size_arr *= 2) {
        // Generate vectors
        std::vector <std::vector <int>> vecs;
        vecs.reserve (repeat);
        for (std::size_t i = 0; i < repeat; ++i) {
            vecs.push_back (getRandFillVector <int> (size_arr, mersenne));
        }
        auto copy_vecs = vecs;

        // Test GPU
        auto gpu_begin = std::chrono::steady_clock::now ();
        for (auto& vec : vecs) {
            sorter.vect_sort (vec.data (), vec.size ());  
        }
        auto gpu_end = std::chrono::steady_clock::now ();

        // Test CPU
        auto cpu_begin = std::chrono::steady_clock::now ();
        for (auto& vec : copy_vecs) {
            std::sort (vec.begin (), vec.end ());
        }
        auto cpu_end = std::chrono::steady_clock::now ();

        for (std::size_t i = 0; i < repeat; ++i) {
            if (copy_vecs[i] != vecs[i]) {
                std::cerr << "Error! copy_vecs_cpu[i] != vecs[i]" << std::endl;
            }
        }

        auto to_ns = [] (auto time) {
            return std::chrono::duration_cast <std::chrono::milliseconds> (time);
        };
        auto gpu_time = to_ns (gpu_end - gpu_begin) / repeat;
        auto cpu_time = to_ns (cpu_end - cpu_begin) / repeat;
        
        std::cout << "Size array: " << std::setw (10) << size_arr << std::endl
                  << tab << "time GPU: " <<  std::setw (10) << gpu_time.count () << std::endl
                  << tab << "time CPU: " <<  std::setw (10) << cpu_time.count () << std::endl;
        
    }
} // testSpeed ()

} // namespace hidra