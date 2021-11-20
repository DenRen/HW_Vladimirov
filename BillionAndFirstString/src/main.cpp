#include <iostream>
#include "xstring.hpp"
#include <vector>

int main () {
    ::meta::basic_xstring <char> str ("hello");
    std::cout << str;
    std::vector <int> v;
}