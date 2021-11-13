#include <iostream>
#include "xstring.hpp"

/*
TODO:
    find (str)
    replaceAll (...)
*/

int main () {
    ::meta::xstring <char> str ("hello");
    std::cout << str;

    str.find ("he");
}