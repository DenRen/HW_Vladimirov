#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "test/test_speed.hpp"
#include "bin/other_func.hpp"

int
main (int argc, char* argv[]) {
    try {
        test_speed::testSpeed (1, 21);
    } catch (cl::Error& exc) {
        std::cerr 
            << "Exception!" << std::endl
            << exc.what () << ", error code: " << exc.err () << std::endl;
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what () << std::endl;
    }
}