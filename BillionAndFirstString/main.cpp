#include <iostream>
#include "xstring.hpp"
#include <vector>

int main () {
    ::meta::xstring <char> str ("hello");
    std::cout << str;
    std::vector <int> v;
}