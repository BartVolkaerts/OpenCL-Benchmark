__kernel void add(__global const float *input, __global float *output,
        __local float *temp, const int size)
{
    const int threatId = get_global_id(0);
    const int localId = get_local_id(0);

    temp[localId] = input[threatId];
    if (threatId < size)
    {
        //for (int i = 0; i < 1000; ++i)
        {
            temp[localId] = temp[localId] + temp[localId];
        }
    }
    output[threatId] = temp[localId];
}

__kernel void addVector4(__global const float4 *input, __global float4 *output,
        __local float4 *temp, const int size)
{
    const int threatId = get_global_id(0);
    const int localId = get_local_id(0);

    temp[localId] = input[threatId];
    if (threatId < size)
    {
        //for (int i = 0; i < 1000; ++i)
        {
            temp[localId] = temp[localId] + temp[localId];
        }
    }
    output[threatId] = temp[localId];
}
