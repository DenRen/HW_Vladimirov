#define SORT_INT4(arr, tmp4, res)                       \
    tmp4 = ((arr).s02 > (arr).s13).s0011;               \
    arr = select ((arr).s0123, (arr).s1032, tmp4);      \
                                                        \
    tmp4 = ((arr).s01 > (arr).s32).s0110;               \
    arr = select ((arr).s0123, (arr).s3210, tmp4);      \
                                                        \
    tmp4 = ((arr).s02 > (arr).s13).s0011;               \
    res = select ((arr).s0123, (arr).s1032, tmp4);      \

void
_sort_int4 (int4 arr, int4* res) {
    int4 tmp;
    SORT_INT4 (arr, tmp, *res)
}

__kernel void
sort_int4 (__global int4* buf) {
    unsigned pos = get_global_id (0);
    int4 arr = buf[pos], tmp;

    SORT_INT4 (arr, tmp, buf[pos])
}

void
_sort_int8 (int8 arr, int8* res) {
    int4 tmp4;

    SORT_INT4 (arr.s0123, tmp4, arr.s0123);
    SORT_INT4 (arr.s4567, tmp4, arr.s4567);

    int8 cmp = (arr.s0123 > arr.s7654).s01233210;
    arr = select (arr, arr.s76543210, cmp);

    SORT_INT4 (arr.s0123, tmp4, res->s0123);
    SORT_INT4 (arr.s4567, tmp4, res->s4567);
}

__kernel void
sort_int8 (__global int8* buf) {
    unsigned pos = get_global_id (0);
    int8 arr = buf[pos];
    
    _sort_int8 (arr, &buf[pos]);
}

void
_sort_int16 (int16 arr, int16* res) {
    int8 res_lo, res_hi;

    _sort_int8 (arr.lo, &res_lo);
    _sort_int8 (arr.hi, &res_hi);

    arr = (int16) (res_lo, res_hi);

    int16 cmp = (arr.s01234567 > arr.sfedcba98).s0123456776543210;
    arr = select (arr, arr.sfedcba9876543210, cmp);

    _sort_int8 (arr.lo, &res_lo);
    _sort_int8 (arr.hi, &res_hi);

    *res = (int16) (res_lo, res_hi);
}

__kernel void
sort_int16 (__global int16* buf) {
    unsigned pos = get_global_id (0);
    _sort_int16 (buf[pos], &buf[pos]);
}

void
reverse_swap_if_less (__global int8* ptr_a, __global int8* ptr_b) {
    int8 a = *ptr_a, b = *ptr_b;

    int16 cmp = (a.s01234567 > b.s76543210).s0123456776543210;
    *ptr_a = select (a, b.s76543210, cmp.lo);
    *ptr_b = select (b, a.s76543210, cmp.hi);
}

// sizeof (int16) * (2 ^ m)
// num_iters = m
__kernel void
sort_mult_int16_pow2 (__global __read_write int16* buf, int m_max) {
    const unsigned pos = get_global_id (0);

    _sort_int16 (buf[pos], &buf[pos]);

    for (int m = 2; m <= m_max; ++m) {

        int k = 1 << (m - 1);
        int M = k << 1;
        int S = pos % k;
        int D = pos / k;

        int ifrom = S + M * D;
        int ito = ifrom + M - 2 * S - 1;

        __global int8* from = ((__global int8*) buf) + ifrom;
        __global int8* to = ((__global int8*) buf) + ito;

        barrier (CLK_LOCAL_MEM_FENCE);
        reverse_swap_if_less (from, to);

        barrier (CLK_LOCAL_MEM_FENCE);
        for (int j = m - 1; j >= 2; --j) {
            k = 1 << (j - 1);
            M = k << 1;
            S = pos % k;
            D = pos / k;

            ifrom = S + M * D;
            ito = ifrom + M - 2 * S - 1;

            __global int8* from = ((__global int8*) buf) + ifrom;
            __global int8* to = ((__global int8*) buf) + ito;

            barrier (CLK_LOCAL_MEM_FENCE);
            reverse_swap_if_less (from, to);
            
            barrier (CLK_LOCAL_MEM_FENCE);
            _sort_int16 (buf[pos], &buf[pos]);
        }
        
        _sort_int16 (buf[pos], &buf[pos]);
    }
}