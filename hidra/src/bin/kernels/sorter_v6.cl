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

// Data should be mult 8 * sizeof (int)
// glob_size == local_size == num_int4 / 2
__kernel void
vector_sort_i4 (__global __read_write int4* g_buf,
                __local int4* l_buf,
                int init_dir)
{
    uint pos = get_global_id (0);
    uint max_size = 2 * get_global_size (0);

    // Sort with dir 8 ints and load it in local memory
    _sort_int8 (*(int8*) &g_buf[2 * pos], (int8*) &l_buf[2 * pos], (pos % 2) ^ init_dir);

    for (uint size = 4; size <= max_size; size <<= 1) {
        const int dir = init_dir ^ (2 * pos / size) % 2; // 0

        // stage = 2, 1
        for (uint stage = size / 2; stage >= 1; stage >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            int4 tmp4;

            uint begin = pos + stage * (pos / stage);
            int8 arr = (int8) (l_buf[begin], l_buf[begin + stage]);

            _sort_int8 (arr, &arr, dir);

            l_buf[begin] = arr.lo;
            l_buf[begin + stage] = arr.hi;
        }
    }

    *(int8*) &g_buf[2 * pos] = *(int8*) &l_buf[2 * pos];
}

void
comparator_i4 (__read_write
               __global int4* g_buf,  // Global buffer with all array
               __local int4* l_buf,   // Local buffer for store full sort block
               uint first,            // Position first block in block_size
               uint second,           // Position second blocl in block_size
               int init_dir)          // Direction sort (0 -> /, 1 -> \)
{
    const uint pos = get_global_id (0);

    const uint block_size = get_global_size (0); // In int4
    const uint max_size = 2 * block_size;        // In int4
    const uint dist = second - first - 1;        // In block_size 

    if (pos < block_size / 2) {
        g_buf += first * block_size;
    } else {
        g_buf += (second - 1) * block_size;
    }

    _sort_int8 (*(int8*) &g_buf[2 * pos], (int8*) &l_buf[2 * pos], (pos % 2) ^ init_dir);

    // Sort with dir 8 ints and load it in local memory
    // const uint offset = (4 * pos > get_global_size (0)) * dist * block_size;
    // _sort_int8 (*(int8*) &g_buf[2 * pos + offset], (int8*) &l_buf[2 * pos], (pos % 2) ^ init_dir);

    for (uint size = 4; size <= max_size; size <<= 1) {
        const int dir = init_dir ^ (2 * pos / size) % 2;

        for (uint stage = size / 2; stage >= 1; stage >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            int4 tmp4;

            uint begin = pos + stage * (pos / stage);
            int8 arr = (int8) (l_buf[begin], l_buf[begin + stage]);

            _sort_int8 (arr, &arr, dir);

            l_buf[begin] = arr.lo;
            l_buf[begin + stage] = arr.hi;
        }
    }

    *(int8*) &g_buf[2 * pos] = *(int8*) &l_buf[2 * pos];
} // comparator_vect_i4

void bitonic_merge (__read_write
                    __global  int4* g_buf, // Global buffer with all array
                    __local int4* l_buf,   // Local buffer for store full sort block
                    uint start,            // Start position in block_size
                    uint size,             // Number block_size that need merge
                    int dir)               // Direction sort (0 -> /, 1 -> \)
{
    const uint max_size = size >> 1;

    while (size >>= 1) {
        const uint stage = max_size / size;

        for (uint j = 0; j < stage; ++j) {
            for (uint i = 0; i < size; ++i) {
                const uint begin = start + i + j * 2 * size;
                comparator_i4 (g_buf, l_buf, begin, begin + size, dir);
                barrier (CLK_LOCAL_MEM_FENCE);
            }
        }
    }
} // bitonic_merge

/**
 * Sorts the array in blocks of 8 * get_global_size (0) ints
 */
__kernel void
big_vector_sort_i4 (__read_write
                    __global int4* g_buf, // Global buffer with all array
                    __local int4* l_buf,  // Local buffer for store full sort block
                    uint arr_size)        // Size array in block_size
{
    for (uint size = 2; size <= arr_size; size <<= 1) {
        int dir = 1;
        for (uint start = 0; start < arr_size; start += size) {
            dir ^= 1;
            bitonic_merge (g_buf, l_buf, start, size, dir);
        }
    }
} // big_vector_sort_i4
