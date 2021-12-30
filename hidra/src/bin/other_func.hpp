#pragma once

#include <vector>
#include <iostream>
#include <fstream>

#include "print_lib.hpp"

static std::string
readSource (std::string fileName) {
    using stream = std::istreambuf_iterator <std::string::traits_type::char_type>;

    std::ifstream file (fileName);
    return std::string (stream (file), stream ());
}

template <typename T>
bool operator != (const std::vector <T>& lhs,
                  const std::vector <T>& rhs)
{
    const std::size_t size = lhs.size ();
    if (size != rhs.size ()) {
        return true;
    }

    for (std::size_t i = 0; i < size; ++i) {
        if (lhs[i] != rhs[i]) {
            return true;
        }
    }

    return false;
} // bool operator != (const std::vector <T>& lhs, const std::vector <T>& rhs)

template <typename T>
bool operator == (const std::vector <T>& lhs,
                  const std::vector <T>& rhs)
{
    const std::size_t size = lhs.size ();
    if (size != rhs.size ()) {
        return false;
    }

    for (std::size_t i = 0; i < size; ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }

    return true;
} // bool operator == (const std::vector <T>& lhs, const std::vector <T>& rhs)

template <typename CharT, typename Traits>
std::basic_ostream <CharT, Traits>&
tab (std::basic_ostream <CharT, Traits>& os) // Output stream
{
    return os.put (os.widen ('\t'));
} // tab (std::basic_ostream <CharT, Traits>& os)

template <typename T, typename Rand>
std::vector <T>
getRandFillVector (std::size_t size,
                   Rand& rand)
{
    std::vector <T> vec (size);
    for (auto& item : vec) {
        item = rand ();
    }

    return vec; // RVO
} // getRandFillVector (std::size_t size, Rand& rand)


template <typename T, typename Rand>
std::vector <T>
getRandFillVector (std::size_t size,
                   Rand& rand,
                   T module)
{
    std::vector <T> vec (size);
    for (auto& item : vec) {
        item = rand () % module;
    }

    return vec; // RVO
} // getRandFillVector (std::size_t size, Rand& rand, T module)

template <typename T>
void
checkEqual (      std::vector <T>& source_vec,
            const std::vector <T>& sorted_vec,
            bool dir = false)
{
    auto copy_src_vec = source_vec;

    if (dir == false) {
        std::sort (source_vec.begin (), source_vec.end ());
    } else {
        std::sort (source_vec.begin (), source_vec.end (),
            [] (int& a, int& b) {
                return a > b;
            }
        );
    }

    ASSERT_TRUE (sorted_vec == source_vec)
        << "src: " << copy_src_vec << std::endl << std::endl
        << "vec: " << sorted_vec   << std::endl << std::endl
        << "ref: " << source_vec   << std::endl << std::endl
        << "dir: " << dir          << std::endl;
} // checkEqual (std::vector <T>& source_vec,
  //             const std::vector <T>& sorted_vec, bool dir = false)
