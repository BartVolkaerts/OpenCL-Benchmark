__kernel void calculate(__read_only image2d_t input,
                        __write_only image2d_t output, 
                        const int2 size)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);
    int2 pos;
    float4 test; 
    const sampler_t samplerA = CLK_NORMALIZED_COORDS_TRUE |
                                CLK_ADDRESS_REPEAT        |
                                CLK_FILTER_NEAREST;

    if(posX >= 200 || posY >= 200)
        return;

    pos = (int2)(posX, posY);
    test = (float4)(1.0f,0.f,0.5f,0.5f);
    write_imagef(output,pos,test);
}

