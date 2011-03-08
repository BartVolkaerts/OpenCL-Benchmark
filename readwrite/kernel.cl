__kernel void byteToFloat(__global float4 *output)
{
    const int posX = get_global_id(0);
    if (posX<10)
    {
        output[posX].x = 0.6f;
        output[posX].y = 0.3f;
    }
}
