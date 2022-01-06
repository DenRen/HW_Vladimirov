#include "find_impl.hpp"

#include "bin/print_lib.hpp"
#include "bin/cppl.hpp"
#include "bin/finder_algo.hpp"

std::tuple <std::string, std::vector <std::string>>
readHaystackAndNeedles (std::istream& is) {
    std::string haystack;
    is >> haystack;
    if (std::cin.fail ()) {
        throw std::invalid_argument ("Failed to input hatstack.");
    }

    std::size_t numberNeedles = 0;
    is >> numberNeedles;
    if (std::cin.fail ()) {
        throw std::invalid_argument ("Failed to inuput number of needles.");
    }

    std::vector <std::string> needles (numberNeedles);
    for (auto& needl : needles) {
        is >> needl;
        if (is.fail ()) {
            throw std::invalid_argument ("Failed to input needle");
        }
    }

    return std::make_tuple (std::move (haystack), std::move (needles));
}

std::vector <int>
numberRepeats (const std::string& haystack,
               const std::vector <std::string>& needles) {
    return numberRepeats_KarpRabin (haystack, needles);
}