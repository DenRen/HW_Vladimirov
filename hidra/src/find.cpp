#include "bin/cppl.hpp"
#include "find_impl.hpp"
#include "bin/finder_algo.hpp"
#include "bin/print_lib.hpp"

int main () {
    try {
        hidra::DeviceProvider deviceProvider;
        auto device = deviceProvider.getDefaultDevice ();
        hidra::Finder finder {device};

        auto[haystack, needles] = readHaystackAndNeedles ();
        // auto repeats = finder.numberRepeats (haystack, needles);

        auto repeats = numberRepeats (haystack, needles);

        printRepeats (repeats);
    } catch (cl::Error& exc) {
        hidra::printError (exc);
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what () << std::endl;
    }
}