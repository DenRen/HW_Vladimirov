#include "clbrod.hpp"

int main () {
    try {
        clbrod::drawFractal (clbrod::QUALITY::FHD, sf::Style::Fullscreen);
    } catch (cl::Error& exc) {
        hidra::printError (exc);
        throw;
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what () << std::endl;
    }
}