#define SORT_INT4(arr, tmp4, res, dir)                          \
    tmp4 = ((arr).s02 > (arr).s13).s0011;                       \
    arr = select ((arr).s0123, (arr).s1032, tmp4 ^ (-dir));     \
                                                                \
    tmp4 = ((arr).s01 > (arr).s32).s0110;                       \
    arr = select ((arr).s0123, (arr).s3210, tmp4 ^ (-dir));     \
                                                                \
    tmp4 = ((arr).s02 > (arr).s13).s0011;                       \
    res = select ((arr).s0123, (arr).s1032, tmp4 ^ (-dir));

#ifdef ENABLE_TESTING
__kernel void
test_sort_int4 (__global __read_write int4* g_arr, int dir) {
    int4 arr = *g_arr, tmp4;
    SORT_INT4 (arr, tmp4, *g_arr, dir);
}
#endif // ENABLE_TESTING

void
_sort_int8 (int8 arr, int8* res, int dir) {
    int4 tmp4;

    SORT_INT4 (arr.s0123, tmp4, arr.s0123, dir);
    SORT_INT4 (arr.s4567, tmp4, arr.s4567, dir);

    int8 cmp = (arr.s0123 > arr.s7654).s01233210;
    arr = select (arr, arr.s76543210, cmp ^ (-dir));

    SORT_INT4 (arr.s0123, tmp4, res->s0123, dir);
    SORT_INT4 (arr.s4567, tmp4, res->s4567, dir);
}


#ifdef ENABLE_TESTING
__kernel void
test_sort_int8 (__global __read_write int8* g_arr, int dir) {
    int8 arr = *g_arr;
    _sort_int8 (arr, &arr, dir);
    *g_arr = arr;
}
#endif // ENABLE_TESTING

void
reverse_swap_if_less_i4 (int4* ptr_a, int4* ptr_b, int dir) {
    int4 a = *ptr_a, b = *ptr_b;

    int8 cmp = (a.s0123 > b.s3210).s01233210;
    *ptr_a = select (a, b.s3210, cmp.lo);
    *ptr_b = select (b, a.s3210, cmp.hi);
}

// sizeof ( /\ ) == 8 * sizeof (int)
void
half_filter_i4 (int pos, int4* data, uint size, int dir) {
    while (size) {
        int8 tmp = (int8) (data[pos], data[pos + size]);
        _sort_int8 (tmp, &tmp, dir);
        data[pos] = tmp.lo;
        data[pos + size] = tmp.hi;

        barrier (CLK_LOCAL_MEM_FENCE);

        uint half_size = size >> 1;
        if (pos >= half_size) {
            pos -= half_size;
            data += size;
        }

        size = half_size;
    }
}

// size param is half data size
void
unifying_network_i4 (uint pos, __local int4* data, uint size, int dir) {
    reverse_swap_if_less_i4 (&data[pos], &data[2 * size - pos - 1], dir);
    barrier (CLK_LOCAL_MEM_FENCE);

    uint half_size = size >> 1;
    if (pos < half_size) {
        half_filter_i4 (pos, data, half_size, dir);
    } else {
        half_filter_i4 (pos - half_size, data + size, half_size, dir);
    }
}

__kernel void
vector_sort_i4 (__global __read_write int4* g_buf,
                __local int4* l_buf, int init_dir) {
    uint pos = get_global_id (0);
    uint max_size = 2 * get_local_size (0);

    // Sort with dir 8 ints and load it in local memory
    _sort_int8 (*(int8*) &g_buf[2 * pos], (int8*) &l_buf[2 * pos], (pos % 2) /*^ init_dir*/);
    barrier (CLK_LOCAL_MEM_FENCE);

    for (uint size = 4; size <= max_size; size <<= 1) {
        const int dir = (2 * pos / size) % 2; // 0

        // stage = 2, 1
        for (uint stage = size / 2; stage >= 1; stage >>= 1) {
            int4 tmp4;

            uint begin = pos + stage * (pos / stage);
            int8 arr = (int8) (l_buf[begin], l_buf[begin + stage]);
            
            _sort_int8 (arr, &arr, dir);

            l_buf[begin] = arr.lo;
            l_buf[begin + stage] = arr.hi;

            barrier (CLK_LOCAL_MEM_FENCE);
        }
        barrier (CLK_LOCAL_MEM_FENCE);
    }
    barrier (CLK_LOCAL_MEM_FENCE);

    *(int8*) &g_buf[2 * pos] = *(int8*) &l_buf[2 * pos];
}