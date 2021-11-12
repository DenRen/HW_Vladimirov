#include <iostream>
#include "xstring.hpp"
#include "test_xstring.hpp"

int main () {
    test::test_all ();
    
    meta::xstring <char> str ("Lol");
    std::cout << str;
    // todo: написать тесты на функции

}