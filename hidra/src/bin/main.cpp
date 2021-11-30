#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "mycllib.h"
#include "cppl.hpp"

void sort_cin ();

template <typename Traits, typename CharT>
std::basic_ostream <CharT, Traits>&
operator << (std::basic_ostream <CharT, Traits>& stream, const cl_int2& value) {
    return stream << "{" << value.s[0] << ", " << value.s[1] << "}";
}

template <typename T, typename Traits, typename CharT>
std::basic_ostream <CharT, Traits>&
operator << (std::basic_ostream <CharT, Traits>& stream, const std::vector <T>& vec) {
    const std::size_t size = vec.size ();
    if (size != 0) {
        for (std::size_t i = 0; i + 1 < size; ++i) {
            stream << vec[i] << ", ";
        }
        stream << vec[size - 1];
    }

    return stream;
}

int
main (int argc, char* argv[]) {
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    // cl::Context context (device);
    // cl::CommandQueue cmd_queue (context);

    std::cout << device_provider.dumpAll ();
    return 0;

    // const std::string source_name = "kernels/sorter_v5.cl";
    // using stream = std::istreambuf_iterator <std::string::traits_type::char_type>;

    // std::ifstream source_file (source_name);
    // std::string source = std::string (stream (source_file), stream ());

    // cl::Program program (context, source);
    // try {
    //     program.build ();
    // } catch (cl::Error& exc) {
    //     cl_int buildError = CL_SUCCESS;

    //     auto buildInfo = program.getBuildInfo <CL_PROGRAM_BUILD_LOG> (&buildError);

    //     for (const auto& pair : buildInfo) {
    //         auto device = pair.first.getInfo <CL_DEVICE_NAME> ();
    //         auto build_error_msg = pair.second;
    //         std::cerr << "Device: " << device << std::endl;
    //         std::cerr << "Build msg: " << build_error_msg << std::endl;
    //     }

    //     throw;
    // }
/*
    std::random_device rd;
    std::mt19937 mersenne(rd());

    const bool print = false;
    const int size_arg = 8;
    std::vector <int> arr ((1 << 10));
    for (auto& item : arr) {
        for (int i = 0; i < size_arg; ++i) {
            item.s[i] = mersenne ();
        }
    }

    if (print) {
        std::cout << "Input:" << std::endl;
        for (auto& item : arr) {
            for (int i = 0; i < size_arg; ++i) {
                std::cout << item.s[i] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    hidra::Sorter sorter (device);
    sorter.vect_sort (arr.data (), arr.size ());

    if (print) {
        std::cout << "result:" << std::endl;
        for (auto& item : arr) {
            for (int i = 0; i < size_arg; ++i) {
                std::cout << item.s[i] << " ";
            }
            std::cout << std::endl;
        }
    }

    std::vector <int> tmp;
    tmp.reserve (size_arg * arr.size ());
    for (const auto& item : arr) {
        for (int i = 0; i < size_arg; ++i) {
            tmp.push_back (item.s[i]);
        }
    }

    std::sort (tmp.begin (), tmp.end ());

    for (int i = 0; i < arr.size (); ++i) {
        for (int j = 0; j < size_arg; ++j) {
            if (arr[i].s[j] != tmp[size_arg * i + j]) {
                std::cout << "False!" << std::endl;
            }
        }
    }*/
}

void
sort_cin () {
    hidra::DeviceProvider device_provider;
    cl::Device device (device_provider.getDefaultDevice ());
    hidra::Sorter sorter (device);

    // --------------------------------------------------------

    std::size_t size = 0;
    std::cin >> size;

    std::cout << "size: " << size << std::endl;

    std::vector <int> arr;
    arr.reserve (size);
    for (std::size_t i = 0; i < size; ++i) {
        int value = 0;
        std::cin >> value;
        arr.push_back (value);
    }

    // --------------------------------------------------------

    sorter.vect_sort (arr);
    for (const auto& value : arr) {
        std::cout << value << " ";
    }

    std::cout << std::endl;
}