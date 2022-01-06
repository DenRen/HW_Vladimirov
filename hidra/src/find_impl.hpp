#pragma once

#include <iostream>
#include <vector>
#include <tuple>

std::tuple <std::string, std::vector <std::string>>
readHaystackAndNeedles (std::istream& is = std::cin);

template <typename T>
void
printRepeats (const std::vector <T>& repeats);

std::vector <int>
numberRepeats (const std::string& haystack,
               const std::vector <std::string>& needles);

template <typename T>
void
printRepeats (const std::vector <T>& repeats) {
    for (std::size_t i = 0; i < repeats.size (); ++i) {
        std::cout << i + 1 << " " << repeats[i] << std::endl;
    }
}