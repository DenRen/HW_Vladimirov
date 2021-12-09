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
_sort_int8 (int8 arr,   // Set of screws to be sorted
            int8* res,  // Pointer to write the result
            int dir)    // Direction sort (0 -> /, 1 -> \)
{
    int4 tmp4;

    SORT_INT4 (arr.s0123, tmp4, arr.s0123, dir);
    SORT_INT4 (arr.s4567, tmp4, arr.s4567, dir);

    int8 cmp = (arr.s0123 > arr.s7654).s01233210;
    arr = select (arr, arr.s76543210, cmp ^ (-dir));

    SORT_INT4 (arr.s0123, tmp4, res->s0123, dir);
    SORT_INT4 (arr.s4567, tmp4, res->s4567, dir);
} // _sort_int8 (int8 arr, int8* res, int dir)

#ifdef ENABLE_TESTING
__kernel void
test_sort_int8 (__read_write
                __global int8* g_arr, // Pointer to the data to be sorted
                int dir)              // Direction sort (0 -> /, 1 -> \)
{
    int8 arr = *g_arr;
    _sort_int8 (arr, &arr, dir);
    *g_arr = arr;
} // test_sort_int8 (__global __read_write int8* g_arr, int dir)
#endif // ENABLE_TESTING

/////////////////////////////////////////////////////////////
// Bitonic sort:                                           //
// ------------                                            //
// https://neerc.ifmo.ru/wiki/index.php?title=Сеть_Бетчера //
/////////////////////////////////////////////////////////////

// Data should be mult 8 * sizeof (int)
// glob_size == local_size == num_int4 / 2
__kernel void
vector_sort_i4 (__read_write
                __global int4* g_buf, // Pointer to the data to be sorted
                __local int4* l_buf,  // Pointer to the local buffer
                                      // for to store the entire array
                int init_dir)         // Direction sort (0 -> /, 1 -> \)
{
    const uint pos = get_global_id (0);
    const uint max_size = 2 * get_global_size (0);

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

// --------------------------------

inline void
Comparator (int* keyA, int* keyB, uint dir) {
    uint t;

    if ((*keyA > *keyB) == dir) {
        t = *keyA;
        *keyA = *keyB;
        *keyB = t;
    }
}

__kernel void
bitonicSortShared (__local int* s_key,
                   __global int* d_DstKey,
                   __global int* d_SrcKey,
                   uint arrayLength,
                   uint dir)
{
    const uint l_buf_size = 2 * get_local_size (0);
    const uint group_id = get_group_id (0);
    const uint local_id = get_local_id (0);

    // Offset to the beginning of subbatch and load data
    d_SrcKey += group_id * l_buf_size + local_id;
    d_DstKey += group_id * l_buf_size + local_id;
    s_key[local_id + 0] = d_SrcKey[0];
    s_key[local_id + (l_buf_size / 2)] = d_SrcKey[(l_buf_size / 2)];

    for (uint size = 2; size < arrayLength; size <<= 1) {
        // Bitonic merge
        uint ddd = dir ^ ((local_id & (size / 2)) != 0);

        for (uint stride = size / 2; stride > 0; stride >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            uint pos = 2 * local_id - (local_id & (stride - 1));
            Comparator(&s_key[pos + 0], &s_key[pos + stride], ddd);
        }
    }

    // ddd == dir for the last bitonic merge step
    {
        for (uint stride = arrayLength / 2; stride > 0; stride >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            uint pos = 2 * local_id - (local_id & (stride - 1));
            Comparator(&s_key[pos + 0], &s_key[pos + stride], dir);
        }
    }

    barrier (CLK_LOCAL_MEM_FENCE);
    d_DstKey[0] = s_key[local_id + 0];
    d_DstKey[(l_buf_size / 2)] = s_key[local_id + (l_buf_size / 2)];
}


////////////////////////////////////////////////////////////////////////////////
// Bitonic sort kernel for large arrays (not fitting into shared memory)
////////////////////////////////////////////////////////////////////////////////
// Bottom-level bitonic sort
// Almost the same as bitonicSortShared with the exception of
// even / odd subarrays being sorted in opposite directions
// Bitonic merge accepts both
// Ascending | descending or descending | ascending sorted pairs
__kernel void
bitonicSortShared1(__local int* s_key,
                   __global int *d_DstKey,
                   __global int *d_SrcKey)
{
    const uint l_buf_size = 2 * get_local_size (0);
    const uint group_id = get_group_id (0);
    const uint local_id = get_local_id (0);

    // Offset to the beginning of subarray and load data
    d_SrcKey += group_id * l_buf_size + local_id;
    d_DstKey += group_id * l_buf_size + local_id;
    s_key[local_id + 0] = d_SrcKey[0];
    s_key[local_id + (l_buf_size / 2)] = d_SrcKey[(l_buf_size / 2)];

    for (uint size = 2; size < l_buf_size; size <<= 1) {
        // Bitonic merge
        uint ddd = (local_id & (size / 2)) != 0;

        for (uint stride = size / 2; stride > 0; stride >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            uint pos = 2 * local_id - (local_id & (stride - 1));
            Comparator(&s_key[pos + 0], &s_key[pos + stride], ddd);
        }
    }

    // Odd / even arrays of l_buf_size elements
    // sorted in opposite directions
    uint ddd = group_id & 1;
    {
        for (uint stride = l_buf_size / 2; stride > 0; stride >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            uint pos = 2 * local_id - (local_id & (stride - 1));
            Comparator(&s_key[pos + 0], &s_key[pos + stride], ddd);
        }
    }

    barrier (CLK_LOCAL_MEM_FENCE);
    d_DstKey[0] = s_key[local_id + 0];
    d_DstKey[(l_buf_size / 2)] = s_key[local_id + (l_buf_size / 2)];
}

// Bitonic merge iteration for stride >= l_buf_size
__kernel void
bitonicMergeGlobal (__global int *d_DstKey,
                    __global int *d_SrcKey,
                    uint arrayLength,
                    uint size,
                    uint stride,
                    uint dir)
{
    const uint l_buf_size = 2 * get_local_size (0);
    const uint group_id = get_group_id (0);
    const uint local_id = get_local_id (0);

    uint global_comparatorI = group_id * get_local_size (0) + local_id;
    uint comparatorI = global_comparatorI & (arrayLength / 2 - 1);

    // Bitonic merge
    uint ddd = dir ^ ((comparatorI & (size / 2)) != 0);
    uint pos = 2 * global_comparatorI - (global_comparatorI & (stride - 1));

    uint keyA = d_SrcKey[pos + 0];
    uint keyB = d_SrcKey[pos + stride];

    Comparator(&keyA, &keyB, ddd);

    d_DstKey[pos + 0] = keyA;
    d_DstKey[pos + stride] = keyB;
}

// Combined bitonic merge steps for
// size > l_buf_size and stride = [1 .. l_buf_size / 2]
__kernel void
bitonicMergeShared(__local int* s_key,
                   __global int *d_DstKey,
                   __global int *d_SrcKey,
                   uint arrayLength,
                   uint size,
                   uint dir)
{
    const uint l_buf_size = 2 * get_local_size (0);
    const uint group_id = get_group_id (0);
    const uint local_id = get_local_id (0);

    d_SrcKey += group_id * l_buf_size + local_id;
    d_DstKey += group_id * l_buf_size + local_id;
    s_key[local_id + 0] = d_SrcKey[0];
    s_key[local_id + (l_buf_size / 2)] = d_SrcKey[(l_buf_size / 2)];

    // Bitonic merge
    uint comparatorI = (group_id * get_local_size (0) + local_id) & ((arrayLength / 2) - 1);
    uint ddd = dir ^ ((comparatorI & (size / 2)) != 0);

    for (uint stride = l_buf_size / 2; stride > 0; stride >>= 1) {
        barrier (CLK_LOCAL_MEM_FENCE);
        uint pos = 2 * local_id - (local_id & (stride - 1));
        Comparator(&s_key[pos + 0], &s_key[pos + stride], ddd);
    }

    barrier (CLK_LOCAL_MEM_FENCE);
    d_DstKey[0] = s_key[local_id + 0];
    d_DstKey[(l_buf_size / 2)] = s_key[local_id + (l_buf_size / 2)];
}