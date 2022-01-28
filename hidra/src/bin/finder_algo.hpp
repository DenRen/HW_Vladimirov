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
