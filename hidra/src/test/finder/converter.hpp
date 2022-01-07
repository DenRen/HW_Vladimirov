#pragma once

#include <vector>
#include <tuple>

namespace cvtr {

namespace detail {

template <std::size_t Pos, typename... Ts>
struct push_back_impl {
    push_back_impl (std::tuple <std::vector <Ts>...>& tuple,
                    const std::tuple <Ts...>& values)
    {
        if constexpr (sizeof... (Ts) == 0) {
            throw "Ts is empty!";
        }

        push_back (tuple, values);
    }

    void push_back (std::tuple <std::vector <Ts>...>& tuple,
                    const std::tuple <Ts...>& values)
    {
        std::get <Pos> (tuple).push_back (
            std::get <Pos> (values)
        );

        push_back_impl <Pos - 1, Ts...> {tuple, values};
    }
};

template <typename... Ts>
struct push_back_impl <0, Ts...> {
    push_back_impl (std::tuple <std::vector <Ts>...>& tuple,
                    const std::tuple <Ts...>& values)
    {
        if constexpr (sizeof... (Ts) == 0) {
            throw "Ts is empty!";
        }

        push_back (tuple, values);
    }

    void push_back (std::tuple <std::vector <Ts>...>& tuple,
                    const std::tuple <Ts...>& values)
    {
        std::get <0> (tuple).push_back (
            std::get <0> (values)
        );
    }
};

} // namespace detail

template <typename... Ts>
void
push_back (std::tuple <std::vector <Ts>...>& tuple,
           const std::tuple <Ts...>& value)
{
    detail::push_back_impl <sizeof... (Ts) - 1, Ts...> {tuple, value};
}

namespace detail {

template <std::size_t Pos, typename... Ts>
struct reserve_impl {
    reserve_impl (std::tuple <std::vector <Ts>...>& tuple,
                  std::size_t size)
    {
        std::get <Pos> (tuple).reserve (size);
        reserve_impl <Pos - 1, Ts...> {tuple, size};
    }
};

template <typename... Ts>
struct reserve_impl <0, Ts...> {
    reserve_impl (std::tuple <std::vector <Ts>...>& tuple,
                  std::size_t size)
    {
        std::get <0> (tuple).reserve (size);
    }
};

} // namespace detail

template <typename... Ts>
void
reserve (std::tuple <std::vector <Ts>...>& tuple,
         std::size_t size)
{
    detail::reserve_impl <sizeof... (Ts) - 1, Ts...> {tuple, size};
}

} // namespace cvtr

/**
 *
 * @brief Inside out vector of tuples.
 * @example Seample case:
 *   input: {{"a", 10}, <- is vector <tuple <Ts...>>
 *           {"b", 20},
 *           {"c", 30}}
 *
 *   output {"a", "b", "c"}, <- is tuple <vector <Ts>...>
 *          { 10,  20,  30}
 */
template <typename... Ts>
std::tuple <std::vector <Ts>...>
turn_out (std::vector <std::tuple <Ts...>>&& vec_tuples) {
    std::tuple <std::vector <Ts>...> res;

    cvtr::reserve (res, vec_tuples.size ());

    for (auto&& tuple : vec_tuples) {
        cvtr::push_back (res, tuple);
    }

    return res;
} // t <T... Ts> turn_out (std::vector <std::tuple <Ts...>>&& vec_tuples)