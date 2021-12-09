#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "cppl.hpp"
#include "other_func.hpp"

void sort_cin ();

// #define TEST_SPEED           // <=== Enable test speed

int
main (int argc, char* argv[]) {
    #ifdef TEST_SPEED
    hidra::testSpeed (1, 21);
    return 0;
    #endif // TEST_SPEED

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

    std::vector <int> arr;
    arr.reserve (size);
    for (std::size_t i = 0; i < size; ++i) {
        int value = 0;
        std::cin >> value;
        arr.push_back (value);
    }

    // --------------------------------------------------------

    sorter.vect_sort (arr);
    std::cout << arr << std::endl;
}
