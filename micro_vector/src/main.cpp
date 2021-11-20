#include <iostream>
#include "mivector.hpp"

int main () {
    meta::mivector <int> vec (4);
    vec.push (1);
    vec.push (2);
    vec.dump ();
}
