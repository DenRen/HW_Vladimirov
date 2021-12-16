#include "clbrod.hpp"

int main () {
    try {
        clbrod::drawFractal (clbrod::QUALITY::FHD, sf::Style::Fullscreen);
    } catch (cl::Error& exc) {
        hidra::printError (exc);
        throw;
    }
}