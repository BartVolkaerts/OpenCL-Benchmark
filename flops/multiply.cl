/*
 * Flops/Integer Benchmark kernel.
 * This kernel test addition.
 * OPERATION16 = 4096 flop
 */
#ifndef TEST_TYPE
#   define TEST_TYPE float
#endif
#ifdef USE_DOUBLE
#   pragma OPENCL EXTENSION cl_khr_fp64: enable
#endif

// 128  Operations
#define OPERATION(x, y) \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \
    y = y * x; \
    x = x * y; \

// 128 * 16 = 2048 Operations

__kernel void multiply(__global TEST_TYPE *buffer,
        const int size, const int iterations)
{
    const int threatId = get_global_id(0);
    const int localId = get_local_id(0);

    TEST_TYPE temp = buffer[threatId];
    if (threatId < size)
    {
        TEST_TYPE x = temp;
        TEST_TYPE y = 1.01f;
        for (int i = 0; i < iterations; ++i)
        {
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
            OPERATION(x, y);
        }
        buffer[localId] = x * y;
    }
    buffer[threatId] = temp;
}
