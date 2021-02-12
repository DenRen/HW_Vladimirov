#include <iostream>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <unistd.h>

//#define MXLIB_DEBUG
#include "MxLib/MxLib.hpp"

using std::cin;
using std::cout;
using std::endl;

int main (int argc, char *argv[]) {
    genmx::Matrix <float> m = genmx::Matrix <float>::eye (3, 3);
    m.SetValue (0, 1, 5);

    cout << m << endl << m.T ();
}