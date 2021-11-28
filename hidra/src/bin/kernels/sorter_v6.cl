#define SORT_INT4(arr, tmp4, res, dir)                  \
    tmp4 = ((arr).s02 > (arr).s13).s0011;               \
    arr = select ((arr).s0123, (arr).s1032, tmp4 ^ (-dir));      \
                                                        \
    tmp4 = ((arr).s01 > (arr).s32).s0110;               \
    arr = select ((arr).s0123, (arr).s3210, tmp4 ^ (-dir));      \
                                                        \
    tmp4 = ((arr).s02 > (arr).s13).s0011;               \
    res = select ((arr).s0123, (arr).s1032, tmp4 ^ (-dir));      \


#ifdef ENABLE_TESTING
__kernel void
test_sort_int4 (__global __read_write int4* g_arr, int dir) {
    int4 arr = *g_arr, tmp4;
    SORT_INT4 (arr, tmp4, *g_arr, dir);
}
#endif // ENABLE_TESTING