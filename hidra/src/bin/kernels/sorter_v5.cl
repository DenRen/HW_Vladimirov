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
_swap_if_greater_int8 (int8 arr, int8* res) {
    int4 tmp4;

    SORT_INT4 (arr.s0123, tmp4, arr.s0123);
    SORT_INT4 (arr.s4567, tmp4, arr.s4567);

    int8 cmp = (arr.s0123 > arr.s7654).s01233210;
    arr = select (arr, arr.s76543210, cmp);

    SORT_INT4 (arr.s0123, tmp4, res->s0123);
    SORT_INT4 (arr.s4567, tmp4, res->s4567);
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
reverse_swap_if_less_i8 (int8* ptr_a, int8* ptr_b) {
    int8 a = *ptr_a, b = *ptr_b;

    int16 cmp = (a.s01234567 > b.s76543210).s0123456776543210;
    *ptr_a = select (a, b.s76543210, cmp.lo);
    *ptr_b = select (b, a.s76543210, cmp.hi);
}

/////////////////////////////////////////////////////////////
// Bitonic sort:                                           //
// ------------                                            //
// https://neerc.ifmo.ru/wiki/index.php?title=Сеть_Бетчера //
/////////////////////////////////////////////////////////////

// size param is half data size
// size % 2 == 0
// pos = [0:size)
void
half_filter (int pos, int8* data, uint size) {
    while (size) {
        int16 tmp = (int16) (data[pos], data[pos + size]);
        _sort_int16 (tmp, &tmp);
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
unifying_network (uint pos, int8* data, uint size) {
    reverse_swap_if_less_i8 (&data[pos], &data[2 * size - pos - 1]);
    barrier (CLK_LOCAL_MEM_FENCE);

    uint half_size = size >> 1;
    if (pos < half_size) {
        half_filter (pos, data, half_size);
    } else {
        half_filter (pos - half_size, data + size, half_size);
    }
}

__kernel void
vector_sort (__global __read_write int8* buf,
             __local int8* l_buf) {
    uint pos = get_global_id (0);
    uint size = get_global_size (0); // Half data size

    _sort_int16 (*(int16*)&buf[2 * pos], (int16*) &l_buf[2 * pos]);

    uint data_size = 2 * size;

    for (uint i = 2; i <= size; i <<= 1) {
        uint global_pos = pos % i + (pos / i) * (2 * i);
        uint new_pos = global_pos % (2 * i);
        __local int8* new_data = l_buf + global_pos - global_pos % i;

        unifying_network (new_pos, new_data, i);
        barrier (CLK_LOCAL_MEM_FENCE);
    }

    *(int16*)&buf[2 * pos] = *(int16*) &l_buf[2 * pos];
}