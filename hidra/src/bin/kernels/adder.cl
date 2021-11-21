// Vector add int argument ----------------------------------------------------

__kernel void vector_add_in_arg_i32 (__global __read_only  int* A,
                                     __global __read_write int* B)
{
    int i = get_global_id (0);
    B[i] = A[i] + B[i];
}

__kernel void vector_add_in_arg_i64 (__global __read_only  long* A,
                                     __global __read_write long* B)
{
    int i = get_global_id (0);
    B[i] = A[i] + B[i];
}

__kernel void vector_add_in_arg_u32 (__global __read_only  unsigned* A,
                                     __global __read_write unsigned* B)
{
    int i = get_global_id (0);
    B[i] = A[i] + B[i];
}

__kernel void vector_add_in_arg_u64 (__global __read_only  unsigned long* A,
                                     __global __read_write unsigned long* B)
{
    int i = get_global_id (0);
    B[i] = A[i] + B[i];
}

// Vector add -----------------------------------------------------------------

__kernel void vector_add_i32 (__global __read_only  int* A,
                              __global __read_only  int* B,
                              __global __write_only int* res)
{
    int i = get_global_id (0);
    res[i] = A[i] + B[i];
}

__kernel void vector_add_i64 (__global __read_only  long* A,
                              __global __read_only  long* B,
                              __global __write_only long* res)
{
    int i = get_global_id (0);
    res[i] = A[i] + B[i];
}

__kernel void vector_add_u32 (__global __read_only  unsigned* A,
                              __global __read_only  unsigned* B,
                              __global __write_only unsigned* res)
{
    int i = get_global_id (0);
    res[i] = A[i] + B[i];
}

__kernel void vector_add_u64 (__global __read_only  unsigned long* A,
                              __global __read_only  unsigned long* B,
                              __global __write_only unsigned long* res)
{
    int i = get_global_id (0);
    res[i] = A[i] + B[i];
}