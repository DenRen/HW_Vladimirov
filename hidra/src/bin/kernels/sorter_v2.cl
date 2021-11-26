#define SORT_INT4(arr, tmp4, res)                   \
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

__kernel void
sort_int16 (__global int16* buf) {
    unsigned pos = get_global_id (0);
    int16 arr = buf[pos];
    int8 res_lo, res_hi;

    _sort_int8 (arr.lo, &res_lo);
    _sort_int8 (arr.hi, &res_hi);

    arr = (int16) (res_lo, res_hi);

    int16 cmp = (arr.s01234567 > arr.sfedcba98).s0123456776543210;
    arr = select (arr, arr.sfedcba9876543210, cmp);

    _sort_int8 (arr.lo, &res_lo);
    _sort_int8 (arr.hi, &res_hi);

    buf[pos] = (int16) (res_lo, res_hi);
}