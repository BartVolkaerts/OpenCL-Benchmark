#define CONSTANT 0.01
#define STAR_CONSTANT 0.01

float4 starInteraction(__global float4 *star,
__global float4 *stars, int numberOfStars);

__kernel void starSimulation(const int numberOfGalacticCenters,
        __global float4 *stars, __global const float4 *galacticCenters,
        __global float4 *starSpeed, const int numberOfStars)
{
    const int id = get_global_id(0);
    // increase speed
    #if 1
    for (int i = 0; i < numberOfGalacticCenters; ++i)
    {
        if (stars[id].x > galacticCenters[i].x)
            starSpeed[id].x -= 1/(stars[id].x - galacticCenters[i].x) * CONSTANT;
        else
            starSpeed[id].x += 1/(galacticCenters[i].x - stars[id].x) * CONSTANT;

        if (stars[id].y > galacticCenters[i].y)
            starSpeed[id].y -= 1/(stars[id].y - galacticCenters[i].y) * CONSTANT;
        else
            starSpeed[id].y += 1/(galacticCenters[i].y - stars[id].y) * CONSTANT;

        if (stars[id].z > galacticCenters[i].z)
            starSpeed[id].z -= 1/(stars[id].z - galacticCenters[i].z) * CONSTANT;
        else
            starSpeed[id].z += 1/(galacticCenters[i].z - stars[id].z) * CONSTANT;
    }
    #endif
    #if 0
    for (int i = 0; i < numberOfStars; ++i)
    {
        if (stars[id].x > stars[i].x)
            starSpeed[i].x -= (1/(stars[id].x - stars[i].x)) * STAR_CONSTANT;
        else
            starSpeed[i].x += (1/(stars[i].x - stars[id].x)) * STAR_CONSTANT;

        if (stars[id].y > stars[i].y)
            starSpeed[i].y -= (1/(stars[id].y - stars[i].y)) * STAR_CONSTANT;
        else
            starSpeed[i].y += (1/(stars[i].y - stars[id].y)) * STAR_CONSTANT;

        if (stars[id].z > stars[i].z)
            starSpeed[i].z -= (1/(stars[id].z - stars[i].z)) * STAR_CONSTANT;
        else
            starSpeed[i].z += (1/(stars[i].z - stars[id].z)) * STAR_CONSTANT;
    }
    #else
    //starSpeed[id].xyz += stars[4].xyz * 0.001;
    #endif

    // position from speed
    stars[id].xyz += starSpeed[id].xyz * 0.001;
}
/*
float4 starInteraction(__global float4 *star,
__global float4 *stars, int numberOfStars)
{
    float4 starSpeed;
    for (int i = 0; i < numberOfStars; ++i)
    {
        if (star->x > stars[i].x)
            starSpeed->x -= 1/(star->x - stars[i].x) * CONSTANT;
        else
            starSpeed->x += 1/(stars[i].x - star->x) * CONSTANT;

        if (star->y > stars[i].y)
            starSpeed->y -= 1/(star->y - stars[i].y) * CONSTANT;
        else
            starSpeed->y += 1/(stars[i].y - star->y) * CONSTANT;

        if (star->z > stars[i].z)
            starSpeed->z -= 1/(star->z - stars[i].z) * CONSTANT;
        else
            starSpeed->z += 1/(stars[i].z - star->z) * CONSTANT;
    }
}*/
