#include <iostream>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include "MxLib/MxLib.hpp"

using std::cin;
using std::cout;
using std::endl;

int main () {
    genmx::Matrix <int> m {3, 3, 5};
    cout << -m << endl;
}