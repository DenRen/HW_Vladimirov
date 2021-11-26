__kernel void
access_in_local_memory (__global int2* buffer, __local int* mem) {
    int global_id = get_global_id (0);

    __local int local_data;

    if (global_id % 2 == 0) {
        buffer[0].s0 = local_data;
        local_data = 5;
    }

    barrier (CLK_LOCAL_MEM_FENCE);

    if (global_id % 2 != 0) {
        buffer[1].s0 = local_data;
        buffer[1].s1 = get_global_id (0);
        local_data *= 7;
    }

    barrier (CLK_LOCAL_MEM_FENCE);

    if (global_id % 2 == 0) {
        buffer[0].s1 = local_data;
    }
}