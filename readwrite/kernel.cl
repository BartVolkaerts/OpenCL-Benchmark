#pragma OPENCL_EXTENSION cl_khr_byte_addressable_store : enable

__kernel void byteToFloat(__global const uchar *data, __global uchar *newdata)
{
    const int posX = get_global_id(0);
    newdata = data;
    if (posX < 20000)
        newdata[posX]-= 100;
}
