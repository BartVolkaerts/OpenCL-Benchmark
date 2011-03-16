__kernel void calculate(__read_only image2d_t input,
                        __write_only image2d_t output, 
                        const int2 size)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);
    int2 pos;
    float4 test; 
    const sampler_t samplerA = CLK_NORMALIZED_COORDS_FALSE;

    if(posX > size.x-1 || posY > size.y-1)
        return;

    pos = (int2)(posX, posY);
    //test = (float4)(1.f ,1.f, 1.f, 1.f);
    test = read_imagef(input, samplerA, pos);
    test.x = 0.f;
    write_imagef(output,pos,test);
}

