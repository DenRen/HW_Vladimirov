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
_merge_int8 (int8 arr,   // Set of screws to be sorted
             int8* res,  // Pointer to write the result
             int dir)    // Direction sort (0 -> /, 1 -> \)
{
    int8 cmp = (arr.s0123 > arr.s7654).s01233210;
    arr = select (arr, arr.s76543210, cmp ^ (-dir));

    int4 tmp4;
    SORT_INT4 (arr.lo, tmp4, res->lo, dir);
    SORT_INT4 (arr.hi, tmp4, res->hi, dir);
} // _sort_int8 (int8 arr, int8* res, int dir)

void
_sort_int8 (int8 arr,   // Set of screws to be sorted
            int8* res,  // Pointer to write the result
            int dir)    // Direction sort (0 -> /, 1 -> \)
{
    int4 tmp4;

    SORT_INT4 (arr.lo, tmp4, arr.lo, dir);
    SORT_INT4 (arr.hi, tmp4, arr.hi, dir);

    int8 cmp = (arr.s0123 > arr.s7654).s01233210;
    arr = select (arr, arr.s76543210, cmp ^ (-dir));

    SORT_INT4 (arr.lo, tmp4, res->lo, dir);
    SORT_INT4 (arr.hi, tmp4, res->hi, dir);
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

#define COMP_i4(first, second, dir) {   \
    int8 __arr = (int8)(first, second); \
    _sort_int8 (__arr, &__arr, !dir);   \
    first  = __arr.lo;                  \
    second = __arr.hi;                  \
} // #define COMP_i4

#define MERGER_i4(first, second, dir) { \
    int8 __arr = (int8)(first, second); \
    _merge_int8 (__arr, &__arr, !dir);  \
    first  = __arr.lo;                  \
    second = __arr.hi;                  \
} // define MERGER_i4

/////////////////////////////////////////////////////////////
// Bitonic sort:                                           //
// ------------                                            //
// https://neerc.ifmo.ru/wiki/index.php?title=Сеть_Бетчера //
/////////////////////////////////////////////////////////////

__kernel void
i4_bitonic_sort_local (__local int4* buf_l,
                       __global int4* buf_g,
                       uint arrayLength,
                       uint dir)
{
    const uint l_buf_size = 2 * get_local_size (0);
    const uint group_id = get_group_id (0);
    const uint local_id = get_local_id (0);

    // Offset to the beginning of subbatch and load data
    buf_g += group_id * l_buf_size + local_id;
    buf_l[local_id + 0] = buf_g[0];
    buf_l[local_id + (l_buf_size / 2)] = buf_g[(l_buf_size / 2)];

    for (uint size = 2; size < arrayLength; size <<= 1) {
        // Bitonic merge
        const uint cur_dir = dir ^ ((local_id & (size / 2)) != 0);

        for (uint stride = size / 2; stride > 0; stride >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            const uint pos = 2 * local_id - (local_id & (stride - 1));
            COMP_i4 (buf_l[pos + 0], buf_l[pos + stride], cur_dir);
        }
    }

    for (uint stride = arrayLength / 2; stride > 0; stride >>= 1) {
        barrier (CLK_LOCAL_MEM_FENCE);
        const uint pos = 2 * local_id - (local_id & (stride - 1));
        COMP_i4 (buf_l[pos + 0], buf_l[pos + stride], dir);
    }

    barrier (CLK_LOCAL_MEM_FENCE);
    buf_g[0] = buf_l[local_id + 0];
    buf_g[(l_buf_size / 2)] = buf_l[local_id + (l_buf_size / 2)];
} // __kernel i4_bitonic_sort_local

__kernel void
i4_bitonic_sort_full_local (__local int4* buf_l,
                            __global int4 *buf_g)
{
    const uint l_buf_size = 2 * get_local_size (0);
    const uint group_id = get_group_id (0);
    const uint local_id = get_local_id (0);

    buf_g += group_id * l_buf_size + local_id;
    buf_l[local_id + 0] = buf_g[0];
    buf_l[local_id + (l_buf_size / 2)] = buf_g[(l_buf_size / 2)];

    for (uint size = 2; size < l_buf_size; size <<= 1) {
        const uint cur_dir = (local_id & (size / 2)) != 0;

        for (uint stride = size / 2; stride > 0; stride >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            const uint pos = 2 * local_id - (local_id & (stride - 1));
            COMP_i4 (buf_l[pos + 0], buf_l[pos + stride], cur_dir);
        }
    }

    const uint cur_dir = group_id & 1;
    for (uint stride = l_buf_size / 2; stride > 0; stride >>= 1) {
        barrier (CLK_LOCAL_MEM_FENCE);
        uint pos = 2 * local_id - (local_id & (stride - 1));
        COMP_i4 (buf_l[pos + 0], buf_l[pos + stride], cur_dir);
    }

    barrier (CLK_LOCAL_MEM_FENCE);
    buf_g[0] = buf_l[local_id + 0];
    buf_g[(l_buf_size / 2)] = buf_l[local_id + (l_buf_size / 2)];
} // __kernel i4_bitonic_sort_full_local

__kernel void
i4_bitonic_merge_global (__global int4* buf_g,
                         uint arrayLength,
                         uint size,
                         uint stride,
                         uint dir)
{
    const uint cmp_g = get_group_id (0) * get_local_size (0) + get_local_id (0);
    const uint cmp = cmp_g & (arrayLength / 2 - 1);

    // Bitonic merge
    const uint cur_dir = dir ^ ((cmp & (size / 2)) != 0);
    const uint pos = 2 * cmp_g - (cmp_g & (stride - 1));

    COMP_i4 (buf_g[pos + 0], buf_g[pos + stride], cur_dir);
} // __kernel i4_bitonic_merge_global

__kernel void
i4_bitonic_merge_local (__local  int4* buf_l,
                        __global int4* buf_g,
                        uint arrayLength,
                        uint size,
                        uint dir)
{
    const uint l_buf_size = 2 * get_local_size (0);
    const uint group_id = get_group_id (0);
    const uint local_id = get_local_id (0);

    buf_g += group_id * l_buf_size + local_id;
    buf_l[local_id + 0] = buf_g[0];
    buf_l[local_id + (l_buf_size / 2)] = buf_g[(l_buf_size / 2)];

    // Bitonic merge
    uint cmp = (group_id * get_local_size (0) + local_id) & ((arrayLength / 2) - 1);
    uint cur_dir = dir ^ ((cmp & (size / 2)) != 0);

    for (uint stride = l_buf_size / 2; stride > 0; stride >>= 1) {
        barrier (CLK_LOCAL_MEM_FENCE);
        uint pos = 2 * local_id - (local_id & (stride - 1));
        MERGER_i4 (buf_l[pos + 0], buf_l[pos + stride], cur_dir);
    }

    barrier (CLK_LOCAL_MEM_FENCE);
    buf_g[0] = buf_l[local_id + 0];
    buf_g[(l_buf_size / 2)] = buf_l[local_id + (l_buf_size / 2)];
} // __kernel i4_bitonic_merge_local