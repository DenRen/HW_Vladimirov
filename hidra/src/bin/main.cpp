#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "mycllib.h"
#include "cppl.hpp"
#include "other_func.hpp"

void sort_cin ();

// #define TEST_SPEED           // <=== Enable test speed
// #define DUMP_ALL_DEVICES     // <=== Dump all info about your ocl devices

int
main (int argc, char* argv[]) {
    #ifdef TEST_SPEED
    hidra::testSpeed ();
    return 0;
    #endif // TEST_SPEED

    #ifdef DUMP_ALL_DEVICES
    hidra::DeviceProvider device_provider;
    cl::Device device = device_provider.getDefaultDevice ();
    std::cout << device_provider.dumpAll ();
    return 0;
    #endif // DUMP_ALL_DEVICES

    sort_cin ();
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