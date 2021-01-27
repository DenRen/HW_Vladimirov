#include <iostream>
#include <cstdio>
#include <ctime>

#include "MxLib.hpp"

int main () {
    const int n = 400;

    genmx::Matrix <int> A (1000, 1000);

    clock_t start_time = clock ();
    for (int i = 0; i < n; i++) {
        auto B = A;
    }
    clock_t finish_time = clock ();

    printf("n: %d,\t%lg seconds\n", n, (double)(finish_time - start_time) / 
                                                CLOCKS_PER_SEC);
}