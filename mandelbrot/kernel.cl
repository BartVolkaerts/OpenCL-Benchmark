#ifdef USE_DOUBLE
#   pragma OPENCL EXTENSION cl_khr_fp64 : enable
#   define CALC_TYPE double
#else
#   define CALC_TYPE float
#endif

#define MIN_REAL -2.0
#define MAX_REAL 1.0
#define MIN_IMAGINARY -1.2
#define MAX_IMAGINARY (minImaginary + (maxReal - minReal) *\
    get_image_height(texture) / get_image_width(texture))
//#define MAX_IMAGINARY 1.2
#define MAX_ITERATIONS 200

#define IMAGINARY_POS(y, height)\
    (CALC_TYPE)(MAX_IMAGINARY -\
    (y) * ((MAX_IMAGINARY - minImaginary) / (CALC_TYPE)((height) - 1)))

#define REAL_POS(x, width)\
    (CALC_TYPE)(minReal + (x) * ((maxReal - minReal) / (CALC_TYPE)((width) - 1)))

__kernel void calculate(__write_only image2d_t texture, const int2 size,
        const int maxIterations, const CALC_TYPE minReal, const CALC_TYPE maxReal,
        const CALC_TYPE minImaginary)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);

    if (posX >= size.x || posY >= size.y)
        return;

    CALC_TYPE imaginaryNumber = IMAGINARY_POS(posY, get_image_height(texture));//getImaginary(posY, get_image_height(texture));
    CALC_TYPE realNumber = REAL_POS(posX, get_image_width(texture));//getReal(posX, get_image_width(texture));
    CALC_TYPE zReal, zImaginary, tmp;
    CALC_TYPE zRealSquared, zImaginarySquared;
    float color;
    bool isInside = true;

    zReal = realNumber;
    zImaginary = imaginaryNumber;

    for (int i = 0; i < maxIterations; ++i)
    {
        zRealSquared = zReal * zReal;
        zImaginarySquared = zImaginary * zImaginary;
        if (zRealSquared + zImaginarySquared > 4)
        {
            isInside = false;
            color = (float)(i * 2) / (float)maxIterations;
            break;
        }
        tmp = zRealSquared - zImaginarySquared + realNumber;
        zImaginary = 2 * zReal * zImaginary + imaginaryNumber;
        zReal = tmp;
    }
    if (isInside)
    {
        write_imagef(texture, (int2)(posX, posY), (float4)(0.f, 0.f, 0.f, 0.5f));
    }
    else
    {
        float4 renderColor = (float4)(0.f, 0.f, 0.f, 0.f);
        // Red
        if (color < 1.f)
            renderColor.x = -color + 1.f;
        // Blue
        if (color > 1.f)
            renderColor.y = (-color + 1.f) + 1.f;
        // Green
        if (color > 0.5f && color < 1.5f)
            renderColor.z = (-color + 0.5f) + 1.f;

        write_imagef(texture, (int2)(posX, posY), renderColor);
    }
}

