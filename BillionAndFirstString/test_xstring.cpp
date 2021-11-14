#include <cassert>
#include <cstring>
#include "xstring.hpp"
#include "gtest/gtest.h"

using cstring = ::meta::xstring <char>;
using size_type = cstring::size_type;

// Не получается достать (see follow TEST example)
const static cstring::size_type temp_npos = cstring::npos;
const static cstring::size_type temp_size_empty = cstring::size_empty;

TEST (XSTRING_TEST, STATIC_CONSTS) {
    // TODO: FIX BUG!
    // undefined reference to `meta::xstring<char>::size_empty'
    // #define GEN_BUG

    #ifdef GEN_BUG
    ASSERT_EQ (cstring::npos, -2);
    ASSERT_EQ (cstring::size_empty, -0);
    #undef GEN_BUG
    #endif

    // Similary with cstring::npos
}

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

TEST (XSTRING_TEST, MOVE_CONSTRUCT) {
    cstring temp ("Hello");
    cstring str (std::move (temp));
}

template <typename T, std::size_t N>
constexpr static cstring::size_type
str_len (const T (&arr)[N]) noexcept {
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
        cstring first (str1);
        first.add (str2, str_len (str2));
        check_on_equal_str (first, res);
    }

    {
        cstring first (str1);
        first += second;
        check_on_equal_str (first, res);
    }
}

template <typename MethodGetSymbol>
static void test_method_at (MethodGetSymbol get_symbol) {
    const char phrase[] = "Hello, world!";
    const cstring::size_type len = str_len (phrase);

    cstring str (static_cast <const char*> (phrase));

    ASSERT_EQ (str.length (), len);
    ASSERT_TRUE (str.is_empty () == false);

    for (cstring::size_type pos = 0; pos < len; ++pos) {
        ASSERT_EQ (get_symbol (str, pos), phrase[pos]);
    }
}

TEST (XSTRING_TEST, METHOD_AT) {
    test_method_at (
        [] (const cstring& xstr, cstring::size_type pos) {
            return xstr.at (pos);
        }
    );
}

TEST (XSTRING_TEST, METHOD_FIND_CHAR) {
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

TEST (XSTRING_TEST, OPERATOR_SQUARE_BRACKETS) {
    test_method_at (
        [] (const cstring& xstr, cstring::size_type pos) {
            return xstr[pos];
        }
    );

    const char from[] = "Hello, world!";
    const char to[]   = "Hello, Elon!";

    ASSERT_LE (str_len (to), str_len (from));

    cstring str (from);
    for (cstring::size_type pos = 0; pos < str.length (); ++pos) {
        str[pos] = to[pos];
        EXPECT_EQ (str[pos], to[pos]);
    }
}

// [begin, end], begin != end
static char*
copy_substring (char* dest, const char* begin, const char* end) {
    assert (begin < end);
    const size_type len = end - begin;

    memcpy (dest, begin, len * sizeof (char));
    dest[len] = '\0';

    return dest;
}

TEST (XSTRING_TEST, METHOD_FIND_STR) {
    const char src[] = "Once upon a time Linus \
        Torvalds was a skinny unknown, just another \
        nerdy Helsinki techie who had been fooling \
        around with computers since childhood. \
        Then he wrote a groundbreaking operating \
        system and distributed it via the Internet \
        -- for free. Today Torvalds is an international \
        folk hero. And his creation LINUX is used by \
        over 12 million people as well as by companies \
        such as IBM.";

    const size_type len = str_len (src);
    const cstring str (src);

    char* sub_str = new char[len];

    const int window = 13;
    ASSERT_TRUE (window > 0 && window < str_len (src));
    for (size_type pos_begin = 0; pos_begin < len - window; ++pos_begin) {
        for (size_type pos_end = pos_begin + window; pos_end < len; ++pos_end) {
            const char* begin = &src[pos_begin];
            const char* end = &src[pos_end];

            ASSERT_TRUE (begin < end);

            const size_type len_sub_str = end - begin;
            copy_substring (sub_str, begin, end);

            // Test for create_string function
            ASSERT_EQ (strncmp (begin, sub_str, len_sub_str), 0)
                << "len_sub_str: " << len_sub_str << std::endl
                << "substr: " << sub_str;

            ASSERT_EQ (strstr (src, sub_str) - src, pos_begin)
                << "substr: " << sub_str;

            ASSERT_EQ (str.find (sub_str), pos_begin)
                << "substr: " << sub_str;
        }
    }

    delete[] sub_str;

}

static void
test_replace_all (const char* src, const char* from, const char* to, const char* res) {
    cstring str (src);

    str.relpace_all (from, to);
    ASSERT_STREQ (res, str.c_str ());
    ASSERT_EQ (std::strlen (res), str.length ())
        << "str.c_str: " << str.c_str () << std::endl
        << "str.len: " << str.length ();
    ASSERT_LE (std::strlen (res), str.capacity ());
}

TEST (XSTRING_TEST, METHOD_REPLACE_ALL_TO_LE_FROM) {
    {
        // Empty string
        cstring str;

        str.relpace_all ("a", "b");
        ASSERT_EQ (str.length (), 0);

        str.relpace_all ("a", "ab");
        ASSERT_EQ (str.length (), 0);

        str.relpace_all ("ab", "b");
        ASSERT_EQ (str.length (), 0);
    }

    {
        // Unchanged replace all
        const char msg[] = "Hello";
        cstring str (msg);

        str.relpace_all ("a", "b");
        ASSERT_STREQ (msg, str.c_str ());

        str.relpace_all ("a", "ab");
        ASSERT_STREQ (msg, str.c_str ());

        str.relpace_all ("ab", "b");
        ASSERT_STREQ (msg, str.c_str ());
    }

    {
        test_replace_all ("", "e", "o", "");
        test_replace_all ("", "e", "ob", "");

        test_replace_all ("Hello", "e", "o", "Hollo");
        test_replace_all ("Hello", "H", "K", "Kello");
        test_replace_all ("Hello", "ll", "_", "He_o");

        test_replace_all ("Elon", "Elon", "Musk", "Musk");
        test_replace_all ("Mark -> Facebook", "Facebook", "Meta", "Mark -> Meta");

        test_replace_all ("aabbbcccc", "bbb", "_", "aa_cccc");
        test_replace_all ("aabbbcccc", "bb",  "_", "aa_bcccc");
        test_replace_all ("aabbbcccc", "b",   "_", "aa___cccc");

        test_replace_all ("aabbbcccc", "cc", "_", "aabbb__");
        test_replace_all ("11111111111111111", "111", "0", "0000011");
    }
}

TEST (XSTRING_TEST, METHOD_REPLACE_ALL_TO_GA_FROM) {
    test_replace_all ("Hello", "e", "alk! Tre", "Halk! Trello");

    test_replace_all ("abbbcccc", "a", "AAA", "AAAbbbcccc");
    test_replace_all ("aabbbcccc", "a", "AAA", "AAAAAAbbbcccc");
    test_replace_all ("aabbbcccc", "b", "________",  "aa________________________cccc");

    test_replace_all ("abbbcccd", "d", "Hello", "abbbcccHello");
    test_replace_all ("abbbcccc", "c", "Hello", "abbbHelloHelloHelloHello");

    test_replace_all ("aaabs00" "bbsbbbb00" "bdbdb00" "db", "00", "Start",
                      "aaabsStartbbsbbbbStartbdbdbStartdb");

    test_replace_all ("a",  "a", "11111111111111111", "11111111111111111");
    test_replace_all ("aa", "a", "11111111111111111", "1111111111111111111111111111111111");

    test_replace_all ("", "a", "11111111111111111", "");
}

TEST (XSTRING_TEST, OPERATOR_EQ) {
    {
        cstring str1, str2;
        EXPECT_TRUE (str1 == str2);
    }

    {
        cstring str1 ("Hello"), str2 ("Hello");
        EXPECT_TRUE (str1 == str2);
    }

    {
        cstring str1, str2;
        EXPECT_FALSE (str1 != str2);
    }

    {
        cstring str1 ("Hello"), str2 ("Hello, world!");
        EXPECT_TRUE (str1 != str2);
    }
}