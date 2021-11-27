#define BITON_VECTOR_SORT(_arr, dir)                \
    cmp = abs (_arr > shuffle (_arr, mask2)) ^ dir; \
    _arr = shuffle (_arr, cmp * 2 + add2);          \
    cmp = abs (_arr > shuffle (_arr, mask1)) ^ dir; \
    _arr = shuffle (_arr, cmp + add1);

#define VECTOR_SWAP(_arr1, _arr2, dir)                  \
    _first = _arr1;                                     \
    _second = _arr2;                                    \
    cmp = (abs (_first > _second) ^ dir) * 4 + add3;    \
    _arr1 = shuffle2 (_first, _second, cmp);            \
    _arr2 = shuffle2 (_second, _first, cmp);

#define SORT_INT4(arr, tmp4, res)                       \
    tmp4 = ((arr).s02 > (arr).s13).s0011;               \
    arr = select ((arr).s0123, (arr).s1032, tmp4);      \
                                                        \
    tmp4 = ((arr).s01 > (arr).s32).s0110;               \
    arr = select ((arr).s0123, (arr).s3210, tmp4);      \
                                                        \
    tmp4 = ((arr).s02 > (arr).s13).s0011;               \
    res = select ((arr).s0123, (arr).s1032, tmp4);      \

__kernel void
bsort_init (__global __read_write int4* g_data,
            __local int4* l_data) {
    int4 _first, _second, tmp4;
    uint4 cmp;
    uint id, dir, global_start, size, stride;

    const uint4 mask1 = (uint4) (1, 0, 3, 2);
    const uint4 swap  = (uint4) (0, 0, 1, 1);
    const uint4 add1  = (uint4) (0, 0, 2, 2);
    
    const uint4 mask2 = (uint4) (2, 3, 0, 1);
    const uint4 add2  = (uint4) (0, 1, 0, 1);
    const uint4 add3  = (uint4) (0, 1, 2, 3);

    id = get_local_id (0) * 2;
    global_start = get_group_id (0) * get_local_size (0) * 2 + id;

    _first  = g_data[global_start];
    _second = g_data[global_start + 1];

    // Sort first vector
    cmp = abs (_first > shuffle (_first, mask1));
    _first = shuffle (_first, cmp ^ swap + add1);
    cmp = abs (_first > shuffle (_first, mask2));
    _first = shuffle (_first, cmp * 2 + add2);
    cmp = abs (_first > shuffle (_first, mask1));
    _first = shuffle (_first, cmp + add1);

    // Sort second vector
    cmp = abs (_second > shuffle (_second, mask1));
    _second = shuffle (_second, cmp ^ swap + add1);
    cmp = abs (_second > shuffle (_second, mask2));
    _second = shuffle (_second, cmp * 2 + add2);
    cmp = abs (_second > shuffle (_second, mask1));
    _second = shuffle (_second, cmp + add1);

    // Swap elements
    dir = get_local_id (0) % 2;
    tmp4 = _first;
    cmp = (abs (_first > _second) ^ dir) * 4 + add3;
    _first = shuffle2 (_first, _second, cmp);
    _second = shuffle2 (_second, tmp4, cmp);

    BITON_VECTOR_SORT (_first, dir);
    BITON_VECTOR_SORT (_second, dir);
    l_data[id] = _first;
    l_data[id + 1] = _second;
/*
    for (size = 2; size < get_local_size (0); size <<= 1) {
        dir = get_local_id (0) / size & 1;

        for (stride = size; stride > 1; stride >>= 1) {
            barrier (CLK_LOCAL_MEM_FENCE);
            id = get_local_id (0) + (get_local_id (0) / stride) * stride;

            VECTOR_SWAP (l_data[id], l_data[id + stride], dir);
        }
        barrier (CLK_LOCAL_MEM_FENCE);

        id = get_local_id (0) * 2;
        _first = l_data[id];
        _second = l_data[id + 1];

        tmp4 = _first;
        cmp = (abs (_first > _second) ^ dir) * 4 + add3;
        _first = shuffle2 (_first, _second, cmp);
        _second = shuffle2 (_second, tmp4, cmp);

        BITON_VECTOR_SORT (_first, dir);
        BITON_VECTOR_SORT (_second, dir);

        l_data[id] = _first;
        l_data[id + 1] = _second;
    }
    
    id = get_global_id (0) % 2;
    for (stride = get_local_size (0); stride > 1; stride >>= 1) {
        barrier (CLK_LOCAL_MEM_FENCE);
        id = get_local_id (0) + (get_local_id (0) / stride) * stride;
        VECTOR_SWAP (l_data[id], l_data[id + stride], dir);
    }
    barrier (CLK_LOCAL_MEM_FENCE);

    id = get_local_id (0) * 2;
    _first = l_data[id];
    _second = l_data[id + 1];

    tmp4 = _first;
    cmp = (abs (_first > _second) ^ dir) * 4 + add3;
    _first = shuffle2 (_first, _second, cmp);
    _second = shuffle2 (_second, tmp4, cmp);

    BITON_VECTOR_SORT (_first, dir);
    BITON_VECTOR_SORT (_second, dir);
*/
    g_data[global_start] = _first;
    g_data[global_start + 1] = _second;
}