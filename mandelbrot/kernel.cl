#define MIN_REAL -2.0
#define MAX_REAL 1.0
#define MIN_IMAGINARY -1.2
//#define MAX_IMAGINARY (MIN_IMAGINARY + (MAX_REAL - MIN_REAL) *\
//    height / width)
#define MAX_IMAGINARY 1.2
#define MAX_ITERATIONS 50

float getImaginary(int y, int height)
{
    return (float)(MAX_IMAGINARY -
        y * (MAX_IMAGINARY - MIN_IMAGINARY) / (float)(height - 1));
}

float getReal(int x, int width)
{
    return (float)(MIN_REAL + x * (MAX_REAL - MIN_REAL) / (float)(width - 1));
}

__kernel void calculate(__write_only image2d_t texture, const int2 size)
{
    const int posX = get_global_id(0);
    const int posY = get_global_id(1);

    if (posX >= size.x || posY >= size.y)
        return;

    float imaginaryNumber = getImaginary(posY, get_image_height(texture));
    float realNumber = getReal(posY, get_image_width(texture));
    float zReal, zImaginary;
    bool isInside = true;

    zReal = realNumber;
    zImaginary = imaginaryNumber;

    for (int i = 0; i < MAX_ITERATIONS; ++i)
    {
        if (zReal * zReal + zImaginary * zImaginary > 4)
        {
            isInside = false;
            break;
        }
        zReal = (zReal * zReal) - (zImaginary * zImaginary) + realNumber;
        zImaginary = 2 * zReal * zImaginary + imaginaryNumber;
    }
    if (isInside)
        write_imageui(texture, (int2)(posX, posY), (uint4)(0, 0, 0, 127));
    else
        write_imageui(texture, (int2)(posX, posY), (uint4)(255, 255, 255, 127));
}

