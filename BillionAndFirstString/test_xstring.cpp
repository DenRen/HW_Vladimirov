#include <cassert>
#include "xstring.hpp"
#include "gtest/gtest.h"

using cstring = ::meta::xstring <char>;

const static cstring::size_type temp_size_empty = -1;
const static cstring::size_type temp_npos = -2;

TEST (XSTRING_TEST, DEFAULT_CONSTRUCT) {
    cstring str;

    ASSERT_EQ (str.length (), temp_size_empty);
    ASSERT_EQ (str.find ('y'), temp_npos);
    ASSERT_EQ (str.find ('\0'), 0);
    ASSERT_EQ (str.at (0), '\0');

    ASSERT_TRUE (str.is_empty ());
}

template <typename T, std::size_t N>
constexpr static std::size_t str_len (const T (&arr)[N]) noexcept {
    return N - 1;
}

TEST (XSTRING_TEST, OTHER_FUNCS_STR_LEN) {
    ASSERT_EQ (str_len ("Hello!"), 6);
    ASSERT_EQ (str_len (""), 0);

    ASSERT_EQ (str_len (L"Hello!"), 6);
    ASSERT_EQ (str_len (L""), 0);
}

TEST (XSTRING_TEST, CONST_CHAR_PTR_CONSTRUCT) {
    const char phrase[] = "Hello, world!";
    const std::size_t len = str_len (phrase);

    cstring str (static_cast <const char*> (phrase));

    ASSERT_EQ (str.length (), len);
    ASSERT_TRUE (str.is_empty () == false);
}


TEST (XSTRING_TEST, METHOD_AT) {
    const char phrase[] = "Hello, world!";
    const cstring::size_type len = str_len (phrase);

    cstring str (static_cast <const char*> (phrase));

    ASSERT_EQ (str.length (), len);
    ASSERT_TRUE (str.is_empty () == false);

    for (cstring::size_type i = 0; i < len; ++i) {
        ASSERT_EQ (str.at (i), phrase[i]);
    }
}


TEST (XSTRING_TEST, STATIC_CONSTS) {
    // TODO: FIX BUG!
    // undefined reference to `meta::xstring<char>::size_empty'
    // ASSERT_EQ (cstring::size_empty, -1);

    // Similary with cstring::npos
}