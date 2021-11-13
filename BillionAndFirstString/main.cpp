#include <iostream>
#include "xstring.hpp"

int main () {
    ::meta::xstring <char> str;
    std::cout << str;

    for (int i = 0; i < 40; ++i) {
        str.add ("Lol");
        std::cout << str << std::endl;
    }
}