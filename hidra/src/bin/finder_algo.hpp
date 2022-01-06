#pragma once

#include <string>
#include <vector>

template <typename T>
std::vector <T>
gen_ptabl (T p, std::size_t size) {
    std::vector <T> ptab;
    ptab.reserve (size);

    T cur_p = 1;
    for (std::size_t i = 1; i < size; ++i) {
        ptab.push_back (cur_p);
        cur_p *= p;
    }

    return ptab;
} // gen_ptabl (T p, std::size_t size)

int karp_rabin (const std::string& s1,
                const std::string& s2,
                const std::vector <unsigned>& ptab);

std::vector <int>
numberRepeats_KarpRabin (const std::string& haystack,
                         const std::vector <std::string>& needle);