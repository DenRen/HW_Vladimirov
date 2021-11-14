#ifndef CHAR_TRAITS_HPP
#define CHAR_TRAITS_HPP

namespace meta {

template <typename CharT>
class base_char_traits {
public:
    static inline CharT
    eof () noexcept {
        return static_cast <CharT> (0);
    }

    static inline bool
    is_equal (const CharT& lhs, const CharT& rhs) noexcept {
        return lhs == rhs;
    }
}; // class base_char_traits

template <typename CharT>
class char_traits : public base_char_traits <CharT> {};

}

#endif // CHAR_TRAITS_HPP