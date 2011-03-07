#define MIN_REAL -2.0
#define MAX_REAL 1.0
#define MIN_IMAGINARY -1.2
#define MAX_IMAGINARY (minImaginary + (maxReal - minReal) *\
    get_image_height(texture) / get_image_width(texture))
//#define MAX_IMAGINARY 1.2
#define MAX_ITERATIONS 200

#define IMAGINARY_POS(y, height)\
    (float)(MAX_IMAGINARY -\
    (y) * ((MAX_IMAGINARY - minImaginary) / (float)((height) - 1)))

#define REAL_POS(x, width)\
    (float)(minReal + (x) * ((maxReal - minReal) / (float)((width) - 1)))

__kernel void calculate(__write_only image2d_t texture, const int2 size,
        const int maxIterations, const float minReal, const float maxReal,
        const float minImaginary)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);

    if (posX >= size.x || posY >= size.y)
        return;

    float imaginaryNumber = IMAGINARY_POS(posY, get_image_height(texture));//getImaginary(posY, get_image_height(texture));
    float realNumber = REAL_POS(posX, get_image_width(texture));//getReal(posX, get_image_width(texture));
    float zReal, zImaginary, tmp;
    float zRealSquared, zImaginarySquared;
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
            renderColor.x = color;
        // Blue
        if (color > 1.f)
            renderColor.y = color - 1.f;
        // Green
        if (color > 0.5f && color < 1.5f)
            renderColor.z = color - 0.5f;

        write_imagef(texture, (int2)(posX, posY), renderColor);
    }
}

