#include <iostream>
#include "xstring.hpp"

/*
TODO:
    replaceAll (...)
*/

int main () {
    ::meta::xstring <char> str ("hello");
    std::cout << str;

    str.find ("he");
}