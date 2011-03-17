#define CAMERA_PLANE_DISTANCE 40.f
int rayIntersectsTriangle(float4 rayOrigin, float4 rayDirection,
        float4 triangle[]);

__kernel void render(
        __write_only image2d_t texture,
        const int2 textureSize,
        __global float4 *object)
{
    int2 pos;
    pos.x = get_global_id(0);
    pos.y = get_global_id(1);

    if (pos.x >= textureSize.x || pos.y >= textureSize.y)
        return;

    float4 cameraRayDir = (float4)(pos.x - (textureSize.x / 2.f),
            pos.y - (textureSize.y / 2.f), - CAMERA_PLANE_DISTANCE, 0.f);
    cameraRayDir = normalize(cameraRayDir);

    float4 origin = (float4)(0.f, 0.f, 0.f, 0.f);
    float4 triangle[3] = { object[0], object[1], object[2] };

    int out;
    if ((out = rayIntersectsTriangle(origin, cameraRayDir, triangle)) == 1)
        write_imagef(texture, pos, (float4)(0.f, 0.f, 0.f, 1.f));
    else if (out == -1)
        write_imagef(texture, pos, (float4)(1.f, 1.f, 0.f, 1.f));
    else if (out == -2)
        write_imagef(texture, pos, (float4)(0.f, 1.f, 0.f, 1.f));
    else
        write_imagef(texture, pos, (float4)(0.f, 0.f, 1.f, 1.f));

}


int rayIntersectsTriangle(float4 rayOrigin, float4 rayDirection,
        float4 triangle[])
{
#if 1
    const float EPSILON = 0.000001f;
    rayDirection = (float4)(0.f, 0.f, 1.f, 0.f);

    // Vector for edges sharing triangle[0]
    float4 edge1 = triangle[1] - triangle[0];
    float4 edge2 = triangle[2] - triangle[0];

    float4 pvec = cross(rayDirection, edge2);
    float det = dot(edge1, pvec);

    if (det > -EPSILON && det < EPSILON)
        return -1;

    float inv_det = 1.f / det;

    float4 tvec = rayOrigin - triangle[0];

    float u = dot(tvec, pvec) * inv_det;
    if (u < 0.f || u > 1.f)
        return -2;

    float4 qvec = cross(tvec, edge1);
    float v = dot(rayDirection, qvec);

    if (v < 0.f || u + v > 1.f)
        return -3;

    float t = dot(edge2, qvec) * inv_det;

    return 1;
#else
    float4 edge1 = triangle[1] - triangle[0];
    float4 edge2 = triangle[2] - triangle[0];

    float4 tvec = rayOrigin - triangle[0];
    float4 pvec = cross(rayDirection, edge2);
    float det = dot(edge1, pvec);
    det = 1.f / det;

    float u = dot(tvec, pvec) * det;

    if (u < 0.f || u > 1.f)
        return 0;

    float4 qvec = cross(tvec, edge1);
    float v = dot(rayDirection, qvec) * det;

    if (v < 0.f || (u + v) > 1.f)
        return 0;

    return 1;

#endif



}
