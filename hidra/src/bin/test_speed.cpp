#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "cppl.hpp"
#include "other_func.hpp"

int
main (int argc, char* argv[]) {
    try {
        hidra::testSpeed (18, 18);
    } catch (cl::Error& exc) {
        std::cerr 
            << "Exception!" << std::endl
            << exc.what () << ", error code: " << exc.err () << std::endl;
    }
}