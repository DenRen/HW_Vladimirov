#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "bin/cppl.hpp"
#include "bin/other_func.hpp"

void sort_cin ();

int
main (int argc, char* argv[]) {
    try {
        sort_cin ();
    } catch (cl::Error& exc) {
        std::cerr 
            << "Error!" << std::endl
            << exc.what () << ", error code: " << exc.err () << std::endl;
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what () << std::endl;
    }
}

bool
check_on_pow_2 (std::size_t num) {
    return (num & (num - 1)) == 0 && num > 0;
}

template <typename T>
std::istream&
operator >> (std::istream& is, std::vector <T>& vec) {
    std::size_t size = 0;
    std::cin >> size;
    
    if (!check_on_pow_2 (size)) {
        throw std::invalid_argument ("N is not a power 2");
    }

    vec.reserve (vec.size () + size);

    for (std::size_t i = 0; i < size; ++i) {
        T value;
        std::cin >> value;
        if (std::cin.fail ()) {
            throw std::invalid_argument ("Failed to input of numbers.");
        }
        vec.emplace_back (std::move (value));
    }

    return is;
}

void
sort_cin () {
    hidra::DeviceProvider device_provider;
    cl::Device device (device_provider.getDefaultDevice ());
    hidra::Sorter sorter (device);

    std::vector <int> arr;
    try {
        std::cin >> arr;
    } catch (std::invalid_argument& exc) {
        const auto example =
            "\nEnter the N - quantity numbers (N = 2 ^ m) and these numbers.\n"
            "Example:\n"
            "4\n"
            "64 21 32 -78\n"
            "\nAfter clicking Enter you will receive:\n"
            "-78 21 32 64\n";

        throw std::invalid_argument (std::string (exc.what ()) + example);
    }

    sorter.sort (arr);
    std::cout << arr << std::endl;
}
