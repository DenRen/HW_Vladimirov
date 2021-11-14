#include <iostream>
#include "xstring.hpp"

int main () {
    ::meta::xstring <char> str ("hello");
    std::cout << str;
}