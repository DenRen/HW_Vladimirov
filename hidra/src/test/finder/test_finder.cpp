#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <type_traits>

#include <gtest/gtest.h>
#include "../../bin/cppl.hpp"
#include "../../bin/other_func.hpp"
#include "../../bin/finder_algo.hpp"
#include "converter.hpp"

namespace static_test {

const std::vector <std::tuple <
                       std::string,
                       std::vector <std::tuple <std::string, int>>
>> quest_answ = {
    {"hello", {{"ll", 1}, {"l", 2}, {"o", 1}}},
    {"fokrey", {{"y", 1}, {"oo", 0}, {"f", 1}}},
    {"ababa", {{"aba", 2}, {"ab", 2}, {"a", 3}, {"b", 2}}},
    {"aaaa", {{"aa", 3}, {"a", 4}}},
    {"aa", {{"aaa", 0}, {"aababababbaba", 0}, {"a", 2}}},
    {"a", {{"aa", 0}, {"a", 1}, {"b", 0}}}
};

}

TEST (TEST_FINDER, STATIC_TEST_REFERENCE_FUNCTION) {
    auto assert_eq = [] (std::string haystack,
                         std::vector <std::tuple <std::string, int>> questions_answers)
    {
        const auto[needles, needle_repeats] = turn_out (std::move (questions_answers));
        ASSERT_EQ (numberRepeats_KarpRabin (haystack, needles), needle_repeats)
            << "haystack: " << haystack << ", "
            << "reference result: " << questions_answers;
    };

    for (const auto& [haystack, needles_answers] : static_test::quest_answ) {
        assert_eq (haystack, needles_answers);
    }
}

TEST (TEST_FINDER, STATIC_TEST) {
    try {
        hidra::DeviceProvider device_provider;
        cl::Device device = device_provider.getDefaultDevice ();
        hidra::Finder finder (device);

        auto assert_eq = [] (hidra::Finder& finder,
                             std::string haystack,
                             std::vector <std::tuple <std::string, int>> questions_answers)
        {
            const auto[needles, needle_repeats] = turn_out (std::move (questions_answers));
            ASSERT_EQ (finder.numberRepeats (haystack, needles), needle_repeats)
                << "haystack: " << haystack << ", "
                << "reference result: " << questions_answers;
        };

        for (const auto& [haystack, needles_answers] : static_test::quest_answ) {
            assert_eq (finder, haystack, needles_answers);
        }

    } catch (cl::Error& exc) {
        hidra::printError (exc);
        throw;
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what () << std::endl;
        throw;
    }
} // TEST (TEST_SORTER, TEST_VECTOR_SORT)

template <typename Engine = std::mt19937>
class RandomGenerator {
    std::random_device rd;
    Engine engine;

    using gen_type = std::result_of_t <Engine ()>;

public:
    RandomGenerator () :
        engine (rd ())
    {}

    gen_type gen () {
        return engine ();
    }

    gen_type gen (gen_type from, gen_type to) {
        return from + gen () % (to - from);
    }

    std::string
    genString (std::size_t size) {
        constexpr auto char_in_gen = sizeof (gen_type);

        std::stringstream stream (std::string (size, '\0'));

        for (std::size_t i = 0; i < size; ++i) {
            char symb = 'a' + gen () % ('z' - 'a' + 1);
            stream << symb;
        }

        return stream.str ();
    }

    std::vector <std::string>
    genStrings (std::size_t min_size,
                std::size_t max_size,
                std::size_t quantity) {
        std::vector <std::string> strs (quantity);

        for (auto& str : strs) {
            const std::size_t size_str = gen (min_size, max_size);
            str = genString (size_str);
        }

        return strs;
    }

};

TEST (TEST_FINDER, TEST_ON_RANDOM_STRINGS) {
    try {
        hidra::DeviceProvider device_provider;
        cl::Device device = device_provider.getDefaultDevice ();
        hidra::Finder finder (device);

        RandomGenerator random;

        const std::size_t size_str_min = 1 << 5;
        const std::size_t size_str_max = 1 << 15;
        const std::size_t quantity_needle_max = 100;
        const std::size_t repeat = 10;

        for (auto _repeat = repeat; _repeat != 0; --_repeat)
        for (auto size_str = size_str_min; size_str <= size_str_max; size_str *= 2) {
            const auto quantity_needle = random.gen (1, quantity_needle_max);
            const auto haystack = random.genString (size_str);
            const auto needles = random.genStrings (1, size_str_min - 2, quantity_needle);

            const auto ref_result = numberRepeats_KarpRabin (haystack, needles);
            const auto finder_result = finder.numberRepeats (haystack, needles);

            ASSERT_EQ (ref_result, finder_result)
                << "haystack: " << haystack
                << "needles: " << needles;
        }

    } catch (cl::Error& exc) {
        hidra::printError (exc);
        throw;
    } catch (std::exception& exc) {
        std::cerr << "Error: " << exc.what () << std::endl;
        throw;
    }
}