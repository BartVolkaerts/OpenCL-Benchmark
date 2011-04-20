#define MASK_WIDTH 3
#define MASK_HEIGHT 3

__kernel void process(__read_only image2d_t input,
                        __write_only image2d_t output, 
                        const int2 size, const int check)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);
    int2 pos;
    float4 inputPixel;
    float4 outputPixel = (float4)0.f;
    const float masker[MASK_HEIGHT][MASK_WIDTH] = {
                                                {0.f,-1.f,0.f},
                                                {-1.f,4.f,-1.f},
                                                {0.f,-1.f,0.f}
                                                };

    const sampler_t samplerA = CLK_NORMALIZED_COORDS_FALSE;

    if(posX >= size.x-MASK_WIDTH/2 || posY >= size.y-MASK_HEIGHT/2 || 
        posX < MASK_WIDTH/2 || posY < MASK_HEIGHT/2)
        return;

    pos = (int2)(posX, posY);
    for(int i = 0; i < MASK_HEIGHT; i++)
    {
        for (int j = 0; j < MASK_WIDTH; j++)
        {
            outputPixel += masker[i][j] * read_imagef(input,samplerA,(int2)(posX-(MASK_WIDTH/2)+j,posY-(MASK_HEIGHT/2)+i));
        }
    }
    if (check)
        outputPixel += read_imagef(input,samplerA,(int2)(posX,posY));

    write_imagef(output,pos,outputPixel);
}

