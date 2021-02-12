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
    genmx::Matrix <float> m {2, 2};
    m.SetValue (0, 0, -1);
    m.SetValue (0, 1, -1);
    m.SetValue (1, 0, -1);

    cout << m << endl << m.Determinant ();
}