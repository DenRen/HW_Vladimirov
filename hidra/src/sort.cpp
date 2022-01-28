#include <bit>
#include "bin/cppl.hpp"
#include "bin/other_func.hpp"
#include "bin/format_string.hpp"

void sort_cin ();

int
main (int argc, char* argv[]) {
    try {
        sort_cin ();
    } catch (cl::Error& exc) {
        hidra::printError (exc);
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what () << std::endl;
    }
}

template <typename T>
std::istream&
operator >> (std::istream& is, std::vector <T>& vec) {
    std::size_t size = 0;
    std::cin >> size;
    if (std::cin.fail ()) {
        throw std::invalid_argument ("Failed to input size.");
    }

    if (bool is_size_power_2 = std::has_single_bit (size);
        !is_size_power_2) {
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
    hidra::Sorter sorter {device};

    std::vector <int> arr;
    try {
        std::cin >> arr;
    } catch (std::invalid_argument& exc) {
        #define NL NEWLINE
        constexpr auto example =
            NL NL
            NL "Enter the N - quantity numbers (N = 2 ^ m) and these numbers."
            NL "Example:"
            NL "4"
            NL "64 21 32 -78"
            NL "After clicking Enter you will receive:"
            NL "-78 21 32 64"
            NL;
        #undef NL

        throw std::invalid_argument (std::string (exc.what ()) + example);
    }

    sorter.sort (arr);
    std::cout << arr << std::endl;
}