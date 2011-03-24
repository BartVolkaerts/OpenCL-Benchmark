float4 getTriangleNormal(__constant float4 triangle[]);

// Based on: http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/
//      Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
__kernel void triangleIntersection(
        __global float4 *cameraRays,
        const float4 cameraOrigin,
        const int2 imageSize,
        __constant float4 *triangleMesh,
        const int offset,
        __global float4 *intersectionPoints,
        __global float4 *intersectionPointNormals,
        const int surfaceId,
        __global int *surfaceIds)
{
    int2 ray;
    ray.x = get_global_id(0);
    ray.y = get_global_id(1);

    // Return if global id is out of image bounds.
    if (ray.x >= imageSize.x || ray.y >= imageSize.y)
        return;

    const float EPSILON = 0.000001f;
    int indexPos = (ray.y * imageSize.y) + ray.x;
    float4 intersectionPoint;

    float4 edge1 = triangleMesh[offset + 1] - triangleMesh[offset];
    float4 edge2 = triangleMesh[offset + 2] - triangleMesh[offset];

    float4 pvec = cross(cameraRays[indexPos], edge2);
    float det = dot(edge1, pvec);

    if (det > -EPSILON && det < EPSILON)
        return;

    float inv_det = 1.f / det;
    float4 tvec = cameraOrigin - triangleMesh[offset];

    float u = dot(tvec, pvec) * inv_det;

    if (u < 0.f || u > 1.f)
        return;

    float4 qvec = cross(tvec, edge1);
    float v = dot(cameraRays[indexPos], qvec) * inv_det;

    if (v < 0.f || (u + v) > 1.f)
        return;

    if (length(intersectionPoints[indexPos]) > length(intersectionPoint) ||
        intersectionPoint.w < 0.f)
    {
        intersectionPoints[indexPos] =
            length(dot(edge2, qvec) * inv_det) * cameraRays[indexPos];

        intersectionPointNormals[indexPos] =
            getTriangleNormal(&triangleMesh[offset]);

        surfaceIds[indexPos] = surfaceId;
    }
}

float4 getTriangleNormal(__constant float4 *triangle)
{
    float4 result = cross(triangle[1] - triangle[0],
            triangle[2] - triangle[1]);
    return normalize(result);
}
