#include <tuple>
#include <iostream>

#include "finder_algo.hpp"

// wo ptab
std::vector <int>
numberRepeats_KarpRabin (const std::string& haystack,
                         const std::vector <std::string>& needles) {
    const unsigned p = 257;
    auto htab = calc_htab <unsigned> (haystack, p);

    std::vector <int> num_repeats (needles.size ());

    auto it_num_repeats = num_repeats.begin ();
    for (const auto& needle : needles) {
        unsigned hash_needle = hash <unsigned> (needle, 0, needle.size (), p);

        for (unsigned i = 0; i + needle.size () <= haystack.size (); ++i)
        {
            unsigned hash = htab[i + needle.size ()] - htab[i];

            if (hash == hash_needle)
            {
                bool ok = true;
                for (unsigned j = 0; j < needle.size (); j++)
                    if (needle[j] != haystack[i + j])
                        ok = false;

                if (ok) {
                    ++*it_num_repeats;
                }
            }
            hash_needle *= p;
        }

        ++it_num_repeats;
    }

    return num_repeats;
}