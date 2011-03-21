__kernel void calculate(__read_only image2d_t input,
                        __write_only image2d_t output, 
                        const int2 size)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);
    int2 pos;
    float4 pixel; 
    const sampler_t samplerA = CLK_NORMALIZED_COORDS_FALSE;

    if(posX > size.x-1 || posY > size.y-1)
        return;

    pos = (int2)(posX, posY);
    //pixel = (float4)(1.f ,1.f, 1.f, 1.f);
    pixel = read_imagef(input, samplerA,pos);
    if(pixel.x > 0.675f && pixel.x < 0.835f &&
        pixel.y > 0.526f && pixel.y < 0.706f &&
        pixel.z > 0.566f && pixel.z < 0.766f) 
    {
        pixel.x = 1.0f;
        pixel.y = 1.0f;
        pixel.z = 1.0f;
    }
    else
    {
        pixel.x = 0.f;
        pixel.y = 0.f;
        pixel.z = 0.f;
    }
    write_imagef(output,pos,pixel);
}

