__kernel void createRays(
        __global float4 *raysFromCamera,
        const int2 imageSize,
        const float planeDistance)
{
    int2 pos;
    pos.x = get_global_id(0);
    pos.y = get_global_id(1);

    // Return if global id is out of image bounds.
    if (pos.x >= imageSize.x || pos.y >= imageSize.y)
        return;

    // 0, 0 lies in the middle of our vision field.
    float4 currentRay = (float4)(
            pos.x - (imageSize.x / 2.f),
            - pos.y + (imageSize.y / 2.f),
            planeDistance, 0.f);

    raysFromCamera[(pos.y * imageSize.y) + pos.x] = normalize(currentRay);
}

