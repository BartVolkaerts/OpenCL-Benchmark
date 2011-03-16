__kernel void calculate(__read_only image2d_t input,__write_only image2d_t
output)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);
    int2 size;
    int2 pos;
    
    const sampler_t samplerA = CLK_NORMALIZED_COORDS_TRUE |
                                CLK_ADDRESS_REPEAT        |
                                CLK_FILTER_NEAREST;

    size.x = get_image_width(input);
    size.y = get_image_height(input);

    if(posX > size.x || posY > size.y)
        return;

    pos = (int2)(posX, posY);
    float4 test;
    //test = read_imagef(input, samplerA, pos);
    test=(float4)(1.f,0.f,1.f,0.5f);
    write_imagef(output,pos,test);
}

