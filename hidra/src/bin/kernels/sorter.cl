
inline void
swap (int* first, int* second) {
    int temp = *first;
    *first = *second;
    *second = temp;
}

inline void
swap_if_greater (int* data, int pos_first, int pos_second) {
    if (data[pos_first] > data[pos_second]) {
        swap (&data[pos_first], &data[pos_second]);
    }
}

void
vector_sort_first (__global __read_write int* A, int pos) {
    pos *= 2;
    swap_if_greater (A, pos, pos + 1);
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
half_filter (int pos, int* data, size_t size) {
    while (size) {
        swap_if_greater (data, pos, pos + size);
        barrier (CLK_LOCAL_MEM_FENCE);

        size_t half_size = size / 2;
        if (pos >= half_size) {
            pos -= half_size;
            data += size;
        }

        size = half_size;
    }
}

// size param is half data size
void
unifying_network (int pos, int* data, size_t size) {
    swap_if_greater (data, pos, 2 * size - pos - 1);
    barrier (CLK_LOCAL_MEM_FENCE);

    int half_size = size / 2;
    if (pos < half_size) {
        half_filter (pos, data, half_size);
    } else {
        half_filter (pos - half_size, data + size, half_size);
    }
}

__kernel void
vector_sort (__global __read_write int* A) {
    int pos = get_global_id (0);
    int size = get_global_size (0); // Half data size

    vector_sort_first (A, pos); // Micro optimization

    int data_size = 2 * size;

    for (int i = 2; i <= size; i *= 2) {
        int global_pos = pos % i + (pos / i) * (2 * i);
        int new_pos = global_pos % (2 * i);
        int* new_data = A + global_pos - global_pos % i;

        unifying_network (new_pos, new_data, i);
        barrier (CLK_LOCAL_MEM_FENCE);
    }
}

// For test ---------------------------------------------------
__kernel void
_half_filter (__global __read_write int* data, ulong size) {
    int pos = get_global_id (0);
    half_filter (pos, data, size);
}

__kernel void
_unifying_network (__global __read_write int* data, ulong size) {
    int pos = get_global_id (0);
    unifying_network (pos, data, size);
}