#include "clbrod.hpp"

int main () {
    try {
        clbrod::drawFractal (clbrod::QUALITY::FHD);
    } catch (cl::Error& exc) {
        hidra::printError (exc);
        throw;
    }
}