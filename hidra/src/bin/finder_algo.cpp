#include <tuple>
#include <iostream>

#include "finder_algo.hpp"

template <typename T>
T
hash (const std::string& s,
      std::string::size_type l,
      std::string::size_type r,
      const std::vector <T> &ptab)
{
    T sum = 0;
    for (auto i = l; i < r; ++i) {
        sum += (s[i] - 'a' + 1) * ptab[i - l];
    }

    return sum;
}

template <typename T>
T
hash (const std::string& s,
      std::string::size_type l,
      std::string::size_type r,
      const T p)
{
    T sum = 0, cur_p = 1;
    for (auto i = l; i < r; ++i) {
        sum += s[i] * cur_p;
        cur_p *= p;
    }

    return sum;
}

template <typename T>
std::vector <T>
calc_htab (const std::string& str,
           const T p)
{
    std::vector <T> htab (str.size () + 1);

    T cur_p = 1;
    for (auto i = 1; i <= str.size (); ++i) {
        htab[i] = htab[i - 1] + str[i - 1] * cur_p;
        cur_p *= p;
    }

    return htab;
} // calc_htab (const std::string& str, const T p)

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