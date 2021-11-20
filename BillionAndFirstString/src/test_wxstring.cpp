#include <cassert>
#include <cstring>
#include <cwchar>

#include "xstring.hpp"
#include "gtest/gtest.h"

using ::meta::cstring;
using ::meta::wstring;
using size_type = wstring::size_type;

const auto eof = ::meta::char_traits <wchar_t>::eof ();

// Не получается достать (see follow TEST example)
const static size_type temp_npos = cstring::npos;
const static size_type temp_size_empty = cstring::size_empty;

TEST (WXSTRING_TEST, DEFAULT_CONSTRUCT) {
    wstring str;

    ASSERT_EQ (str.length (), temp_size_empty);
    ASSERT_EQ (str.find (L'y'), temp_npos);
    ASSERT_EQ (str.find (eof), 0);
    ASSERT_EQ (str.at (0), eof);

    ASSERT_TRUE (str.is_empty ());
}

TEST (WXSTRING_TEST, COPY_CONSTRUCT) {
    wstring str (L"Lol");
    wstring copt_str (str);

    ASSERT_TRUE (str == copt_str);
}

TEST (WXSTRING_TEST, MOVE_CONSTRUCT) {
    wstring temp (L"Hello");
    wstring str (std::move (temp));
}

template <typename T, std::size_t N>
constexpr static size_type
str_len (const T (&arr)[N]) noexcept {
    return N - 1;
}

TEST (WXSTRING_TEST, METHOD_RESERVE) {
    const wchar_t greeting[] = L"Hello";
    wstring str (greeting);
    
    for (size_type i = 1; i < 50'000'00; i *= 2) {
        // str.reserve (i * str_len (greeting));
        // ASSERT_STREQ ((char*) str.c_str (), (char*) greeting);
    }
}

TEST (WXSTRING_TEST, CONST_CHAR_PTR_CONSTRUCT) {
    const wchar_t phrase[] = L"Hello, world!";
    const size_type len = str_len (phrase);

    wstring str (static_cast <const wchar_t*> (phrase));

    ASSERT_EQ (str.length (), len);
    ASSERT_TRUE (str.is_empty () == false);
}

template <typename CharT, std::size_t N>
static void
check_on_equal_str (const ::meta::basic_xstring <CharT>& xstr, const CharT (&ref)[N]) {
    const auto len_str_ref = str_len (ref);

    ASSERT_EQ (xstr.length (), len_str_ref);

    ASSERT_TRUE (strncmp (
        (const char*) xstr.c_str (), (const char*) ref, len_str_ref * sizeof (CharT)
    ) == 0) << "first.c_str: \"" << xstr.c_str () << "\"";
}

TEST (WXSTRING_TEST, METHOD_ADD) {
    const wchar_t str1[] = L"Hello, ";
    const wchar_t str2[] = L"world!";
    const wchar_t res[]  = L"Hello, world!";

    wstring second (str2);

    {
        wstring first (str1);
        first.add (second);
        check_on_equal_str (first, res);
    }

    {
        wstring first (str1);
        first.add (str2, str_len (str2));
        check_on_equal_str (first, res);
    }

    {
        wstring first (str1);
        first += second;
        check_on_equal_str (first, res);
    }
}

template <typename MethodGetSymbol>
static void test_method_at (MethodGetSymbol get_symbol) {
    const wchar_t phrase[] = L"Hello, world!";
    const size_type len = str_len (phrase);

    wstring str (static_cast <const wchar_t*> (phrase));

    ASSERT_EQ (str.length (), len);
    ASSERT_TRUE (str.is_empty () == false);

    for (size_type pos = 0; pos < len; ++pos) {
        ASSERT_EQ (get_symbol (str, pos), phrase[pos]);
    }
}

TEST (WXSTRING_TEST, METHOD_AT) {
    test_method_at (
        [] (const wstring& xstr, size_type pos) {
            return xstr.at (pos);
        }
    );
}

TEST (WXSTRING_TEST, METHOD_FIND_CHAR) {
    const wchar_t res[] = L"Hello, world!";
    const wchar_t unused_symbols[] = L"1234567890\'\"`()-=q_-+zxcvbnm";

    const auto count_unused_symbols = str_len (unused_symbols) - 1;
    wstring str (res);

    for (size_type i = 0; i < count_unused_symbols; ++i) {
        char unused_symbol = unused_symbols[i];
        // Test for test
        ASSERT_EQ (strchr ((const char*) res, unused_symbol), static_cast <char*> (NULL));

        EXPECT_EQ (str.find (unused_symbol), temp_npos);
    }
}

TEST (WXSTRING_TEST, OPERATOR_SQUARE_BRACKETS) {
    test_method_at (
        [] (const wstring& xstr, size_type pos) {
            return xstr[pos];
        }
    );

    const wchar_t from[] = L"Hello, world!";
    const wchar_t to[]   = L"Hello, Elon!";

    ASSERT_LE (str_len (to), str_len (from));

    wstring str (from);
    for (size_type pos = 0; pos < str.length (); ++pos) {
        str[pos] = to[pos];
        EXPECT_EQ (str[pos], to[pos]);
    }
}

// [begin, end], begin != end
static wchar_t*
copy_substring (wchar_t* dest, const wchar_t* begin, const wchar_t* end) {
    assert (begin < end);
    const size_type len = end - begin;

    std::copy (begin, begin + len, dest);
    dest[len] = '\0';

    return dest;
}

TEST (WXSTRING_TEST, METHOD_FIND_STR_EXIST_SUBSTR) {
    const wchar_t src[] = L"Once upon a time Linus "
                            "Torvalds was a skinny unknown, just another "
                            "nerdy Helsinki techie who had been fooling "
                            "around with computers since childhood. "
                            "Then he wrote a groundbreaking operating "
                            "system and distributed it via the Internet "
                            "-- for free. Today Torvalds is an international "
                            "folk hero. And his creation LINUX is used by "
                            "over 12 million people as well as by companies "
                            "such as IBM.";

    const size_type len = str_len (src);
    const wstring str (src);

    wchar_t* sub_str = new wchar_t[len];

    const int window = 13;
    ASSERT_TRUE (window > 0 && window < str_len (src));
    for (size_type pos_begin = 0; pos_begin < len - window; ++pos_begin) {
        for (size_type pos_end = pos_begin + window; pos_end < len; ++pos_end) {
            const wchar_t* begin = &src[pos_begin];
            const wchar_t* end = &src[pos_end];

            ASSERT_TRUE (begin < end);

            const size_type len_sub_str = end - begin;
            copy_substring (sub_str, begin, end);

            // Test for create_string function
            ASSERT_EQ (wcsncmp (begin, sub_str, len_sub_str), 0)
                << "len_sub_str: " << len_sub_str << std::endl
                << "substr: " << sub_str;

            ASSERT_EQ (wcsstr (src, sub_str) - src, pos_begin)
                << "substr: " << sub_str;

            ASSERT_EQ (str.find (sub_str), pos_begin)
                << "substr: " << sub_str;
        }
    }

    delete[] sub_str;
}


TEST (WXSTRING_TEST, METHOD_FIND_STR_NOT_EXIST_SUBSTR) {
    const wchar_t src[] = L"Once upon a time Linus ...";
    wstring str (src);

    ASSERT_EQ (str.find (L"100"), temp_npos);
    ASSERT_EQ (str.find (L"fg"), temp_npos);
    ASSERT_EQ (str.find (L"1"), temp_npos);
    ASSERT_EQ (str.find (L"Once upon a time Linus ...!"), temp_npos);
}

static void
test_replace_all (const wchar_t* src, const wchar_t* from, const wchar_t* to, const wchar_t* res) {
    wstring str (src);

    str.relpace_all (from, to);
    ASSERT_STREQ (res, str.c_str ());
    ASSERT_EQ (std::wcslen (res), str.length ())
        << "str.c_str: " << str.c_str () << std::endl
        << "str.len: " << str.length ();
    ASSERT_LE (std::wcslen (res), str.capacity ());
}

TEST (WXSTRING_TEST, METHOD_REPLACE_ALL_TO_LE_FROM) {
    {
        // Empty string
        wstring str;

        str.relpace_all (L"a", L"b");
        ASSERT_EQ (str.length (), 0);

        str.relpace_all (L"a", L"ab");
        ASSERT_EQ (str.length (), 0);

        str.relpace_all (L"ab", L"b");
        ASSERT_EQ (str.length (), 0);
    }

    {
        // Unchanged replace all
        const wchar_t msg[] = L"Hello";
        wstring str (msg);

        str.relpace_all (L"a", L"b");
        ASSERT_STREQ (msg, str.c_str ());

        str.relpace_all (L"a", L"ab");
        ASSERT_STREQ (msg, str.c_str ());

        str.relpace_all (L"ab", L"b");
        ASSERT_STREQ (msg, str.c_str ());
    }

    {
        test_replace_all (L"", L"e", L"o", L"");
        test_replace_all (L"", L"e", L"ob", L"");

        test_replace_all (L"Hello", L"e", L"o", L"Hollo");
        test_replace_all (L"Hello", L"H", L"K", L"Kello");
        test_replace_all (L"Hello", L"ll", L"_", L"He_o");

        test_replace_all (L"Elon", L"Elon", L"Musk", L"Musk");
        test_replace_all (L"Mark -> Facebook", L"Facebook", L"Meta", L"Mark -> Meta");

        test_replace_all (L"aabbbcccc", L"bbb", L"_", L"aa_cccc");
        test_replace_all (L"aabbbcccc", L"bb",  L"_", L"aa_bcccc");
        test_replace_all (L"aabbbcccc", L"b",   L"_", L"aa___cccc");

        test_replace_all (L"aabbbcccc", L"cc", L"_", L"aabbb__");
        test_replace_all (L"11111111111111111", L"111", L"0", L"0000011");
    }
}

TEST (WXSTRING_TEST, METHOD_REPLACE_ALL_TO_GA_FROM) {
    test_replace_all (L"Hello", L"e", L"alk! Tre", L"Halk! Trello");

    test_replace_all (L"abbbcccc", L"a", L"AAA", L"AAAbbbcccc");
    test_replace_all (L"aabbbcccc", L"a", L"AAA", L"AAAAAAbbbcccc");
    test_replace_all (L"aabbbcccc", L"b", L"________",  L"aa________________________cccc");

    test_replace_all (L"abbbcccd", L"d", L"Hello", L"abbbcccHello");
    test_replace_all (L"abbbcccc", L"c", L"Hello", L"abbbHelloHelloHelloHello");

    test_replace_all (L"aaabs00" "bbsbbbb00" "bdbdb00" "db", L"00", L"Start",
                      L"aaabsStartbbsbbbbStartbdbdbStartdb");

    test_replace_all (L"a",  L"a", L"11111111111111111", L"11111111111111111");
    test_replace_all (L"aa", L"a", L"11111111111111111", L"1111111111111111111111111111111111");

    test_replace_all (L"", L"a", L"11111111111111111", L"");
}

TEST (WXSTRING_TEST, OPERATOR_EQ) {
    {
        wstring str1, str2;
        EXPECT_TRUE (str1 == str2);
    }

    {
        wstring str1 (L"Hello"), str2 (L"Hello");
        EXPECT_TRUE (str1 == str2);
    }

    {
        wstring str1, str2;
        EXPECT_FALSE (str1 != str2);
    }

    {
        wstring str1 (L"Hello"), str2 (L"Hello, world!");
        EXPECT_TRUE (str1 != str2);
    }
}