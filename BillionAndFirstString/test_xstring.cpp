#include <cassert>
#include <cstring>
#include "xstring.hpp"
#include "gtest/gtest.h"

using cstring = ::meta::xstring <char>;

const static cstring::size_type temp_size_empty = cstring::size_empty;
const static cstring::size_type temp_npos = cstring::npos;

TEST (XSTRING_TEST, DEFAULT_CONSTRUCT) {
    cstring str;

    ASSERT_EQ (str.length (), temp_size_empty);
    ASSERT_EQ (str.find ('y'), temp_npos);
    ASSERT_EQ (str.find ('\0'), 0);
    ASSERT_EQ (str.at (0), '\0');

    ASSERT_TRUE (str.is_empty ());
}

TEST (XSTRING_TEST, COPY_CONSTRUCT) {
    cstring str;

    ASSERT_EQ (str.length (), temp_size_empty);
    ASSERT_EQ (str.find ('y'), temp_npos);
    ASSERT_EQ (str.find ('\0'), 0);
    ASSERT_EQ (str.at (0), '\0');

    ASSERT_TRUE (str.is_empty ());
}

template <typename T, std::size_t N>
constexpr static cstring::size_type str_len (const T (&arr)[N]) noexcept {
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

template <typename CharT, std::size_t N>
static void
check_on_equal_str (const ::meta::xstring <CharT>& xstr, const CharT (&ref)[N]) {
    const auto len_str_ref = str_len (ref);

    ASSERT_EQ (xstr.length (), len_str_ref);

    ASSERT_TRUE (strncmp (xstr.c_str (), ref, len_str_ref * sizeof (CharT)) == 0)
        << "first.c_str: \"" << xstr.c_str () << "\"";
}

TEST (XSTRING_TEST, METHOD_ADD) {
    const char str1[] = "Hello, ";
    const char str2[] = "world!";
    const char res[]  = "Hello, world!";

    cstring second (str2);

    {
        cstring first (str1);
        first.add (second);
        check_on_equal_str (first, res);
    }

    {
        // cstring first (str1);
        // first.add (str2, str_len (str2));
        // check_on_equal_str (first, res);
    }

    {
        // cstring first (str1);
        // first += second;
        // check_on_equal_str (first, res);
    }
}

TEST (XSTRING_TEST, METHOD_FIND) {
    const char res[] = "Hello, world!";
    const char unused_symbols[] = "1234567890\'\"`()-=q_-+zxcvbnm";

    const auto count_unused_symbols = str_len (unused_symbols) - 1;
    cstring str (res);

    for (std::size_t i = 0; i < count_unused_symbols; ++i) {
        char unused_symbol = unused_symbols[i];
        // Test for test
        ASSERT_EQ (strchr (res, unused_symbol), static_cast <char*> (NULL));

        EXPECT_EQ (str.find (unused_symbol), temp_npos);
    }
}