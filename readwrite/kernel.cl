__kernel void calculate(__write_only image2d_t texture)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);
    int2 dims;
    
    dims = get_image_dim(texture);
}

