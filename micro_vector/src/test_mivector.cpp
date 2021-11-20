#include "gtest/gtest.h"
#include "mivector.hpp"
#include <cstdio>

using size_type = typename meta::mivector <int>::size_type;

TEST (MIVECTOR, BASIC) {
    meta::mivector <int> vec (2);

    const int val = 3;
    vec.push (val);
    ASSERT_EQ (vec.top (), val);
    ASSERT_EQ (vec.size (), 1);
    ASSERT_GE (vec.capacity (), 1);
}

TEST (MIVECTOR, PUSH_POP) {
    const size_type init_size = 30;
    const long max_iter = 10000;

    meta::mivector <size_type> vec (init_size);
    long i = 0;
    for (; i < max_iter; ++i) {
        vec.push (i);
    }

    for (; i > 0; --i) {
        ASSERT_EQ (vec.top (), i - 1);
        vec.pop ();
    }
}

TEST (MIVECTOR, COPY_CTOR) {
    const size_type init_size = 30;
    const long max_iter = 10000;

    meta::mivector <long> vec (init_size);
    long i = 0;
    for (; i < max_iter; ++i) {
        vec.push (i);
    }

    meta::mivector <long> copy_vec (vec);

    for (; i > 0; --i) {
        ASSERT_EQ (copy_vec.top (), i - 1);
        copy_vec.pop ();
    }
}

