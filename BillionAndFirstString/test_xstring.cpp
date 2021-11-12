#include "test_xstring.hpp"
#include "xstring.hpp"
#include <cassert>

namespace test {

using cstring = meta::xstring <char>;

static void test_create () {
    cstring str;
    assert (str.length () == cstring::size_empty);
    assert (str.is_empty ());
}

void test_all () {
    std::cout << "Test started" << std::endl;
    test_create ();
    std::cout << "Test finished" << std::endl;
}

}