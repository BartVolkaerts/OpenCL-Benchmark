#define MIN_REAL -2.0
#define MAX_REAL 1.0
#define MIN_IMAGINARY -1.2
#define MAX_IMAGINARY (MIN_IMAGINARY + (MAX_REAL - MIN_REAL) *\
    get_image_height(texture) / get_image_width(texture))
//#define MAX_IMAGINARY 1.2
#define MAX_ITERATIONS 150

#define IMAGINARY_POS(y, height)\
    (float)(MAX_IMAGINARY -\
    (y) * ((MAX_IMAGINARY - MIN_IMAGINARY) / (float)((height) - 1)))

#define REAL_POS(x, width)\
    (float)(MIN_REAL + (x) * ((MAX_REAL - MIN_REAL) / (float)((width) - 1)))

__kernel void calculate(__write_only image2d_t texture, const int2 size)
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

    for (int i = 0; i < MAX_ITERATIONS; ++i)
    {
        zRealSquared = zReal * zReal;
        zImaginarySquared = zImaginary * zImaginary;
        if (zRealSquared + zImaginarySquared > 4)
        {
            isInside = false;
            color = (float)i / MAX_ITERATIONS;
            break;
        }
        tmp = zRealSquared - zImaginarySquared + realNumber;
        zImaginary = 2 * zReal * zImaginary + imaginaryNumber;
        zReal = tmp;
    }
    if (isInside)
        write_imagef(texture, (int2)(posX, posY), (float4)(0.f, 0.f, 0.f, 0.5f));
    else
        write_imagef(texture, (int2)(posX, posY), (float4)(color, color, color, 0.5f));
}

