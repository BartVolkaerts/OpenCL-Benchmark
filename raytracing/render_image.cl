__kernel void renderImage(
        __write_only image2d_t glTexture,
        const int2 imageSize,
        __global float4 *intersectionPoints,
        __global float4 *intersectionPointNormals,
        const float4 emissionSource,
        __global float4 *surfaceIds)
{
    int2 pos;
    pos.x = get_global_id(0);
    pos.y = get_global_id(1);

    // Return if global id is out of image bounds.
    if (pos.x >= imageSize.x || pos.y >= imageSize.y)
        return;

    int indexPos = (pos.y * imageSize.y) + pos.x;

    float4 emissionDir =
        normalize(emissionSource - intersectionPoints[indexPos]);

    float dotProduct = dot(intersectionPointNormals[indexPos], emissionDir);

    float4 diff = dotProduct * (float4)(0.f, 1.f, 1.f, 1.f);
    write_imagef(glTexture, pos, (float4)(1.f, 1.f, 1.f, 1.f));


}
