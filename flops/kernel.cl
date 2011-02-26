/*
 * Flops Benchmark kernel.
 * OPERATION16 = 4096 flop
 */

/*
 * TODO:
 *  - Constant geheugen.
 *  - Overdracht Shared -> Global memory verwaarloosbaar.
 *  - Timing in kernel zelf
 */

// 128 * 2 Operations
#define OPERATION(x, y) \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \
    y = x * y + x; \
    x = y * x + y; \

// 128 * 2 * 16 = 4096 Operations
/*
#define OPERATION16(x, y)\
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
    OPERATION; \
#endif
*/

__kernel void add(__global float *output,
        __local float *temp, const int size, const int iterations)
{
    const int threatId = get_global_id(0);
    const int localId = get_local_id(0);

    temp[localId] = output[threatId];
    if (threatId < size)
    {
        float x = temp[localId];
        float y = 1.01f;

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
        temp[localId] = x + y;
    }
    output[threatId] = temp[localId];
}

__kernel void addVector4(__global float4 *output,
        __local float4 *temp, const int size, const int iterations)
{
    const int threatId = get_global_id(0);
    const int localId = get_local_id(0);

    temp[localId] = output[threatId];
    if (threatId < size)
    {
        float4 x = temp[localId];
        float4 y = (1.01f, -1.01f, 2.002f, 3.0014f);

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
        temp[localId] = x + y;
    }
    output[threatId] = temp[localId];
}
