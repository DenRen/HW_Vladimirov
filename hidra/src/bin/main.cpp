#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "cppl.hpp"
#include "other_func.hpp"

void sort_cin ();

int
main (int argc, char* argv[]) {
    try {
        sort_cin ();
    } catch (cl::Error& exc) {
        std::cerr 
            << "Exception!" << std::endl
            << exc.what () << ", error code: " << exc.err () << std::endl;
    }
}

void
sort_cin () {
    hidra::DeviceProvider device_provider;
    cl::Device device (device_provider.getDefaultDevice ());
    hidra::Sorter sorter (device);

    // --------------------------------------------------------

    std::size_t size = 0;
    std::cin >> size;

    std::vector <int> arr;
    arr.reserve (size);
    for (std::size_t i = 0; i < size; ++i) {
        int value = 0;
        std::cin >> value;
        arr.push_back (value);
    }

    // --------------------------------------------------------

    sorter.sort (arr);
    std::cout << arr << std::endl;
}
