__kernel void add(__global const float *input, __global float *output, const int size)
{
    const int threatId = get_global_id(0);

    if (threatId < size)
    {
        output[threatId] = input[threatId] * input[threatId] + input[threatId];
    }
}

