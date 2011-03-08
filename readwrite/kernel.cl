__kernel void byteToFloat(__write_only image2d_t texture)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);
}
