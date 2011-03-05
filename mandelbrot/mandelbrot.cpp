#include "mandelbrot.h"
#include <QLabel>

Mandelbrot::Mandelbrot(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new MandelbrotMainWidget(parent);
    _configWidget = new QLabel("Mandelbrot", parent);


}

Mandelbrot::~Mandelbrot()
{
}

void Mandelbrot::initCL()
{
    cl_int error;
    _environment->createGLContext();
    _environment->createProgram(QStringList("mandelbrot/kernel.cl"));
    _kernel = _environment->getKernel("calculate");

    // CL Image from GL texture
    _texture = clCreateFromGLTexture2D(_environment->getContext(),
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            _mainWidget->getTexture(), &error);
    CHECK_ERR(error);
}

void Mandelbrot::releaseCL()
{
    //CHECK_ERR(clReleaseMemObject(_texture));
}

void Mandelbrot::execute()
{
    initCL();
    cl_int2 size;
    size.x = _mainWidget->width();
    size.y = _mainWidget->height();


    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_texture));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_int2), &size));

    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 1,
                &_texture, 0, NULL, NULL));

    const size_t localWorkSize[2] = {64, 64};
    const size_t totalWorkItems[2] = {
        (_mainWidget->width() / localWorkSize[0] + 1) * localWorkSize[0],
        (_mainWidget->height() / localWorkSize[1] + 1) * localWorkSize[1]
    };
    qDebug() << totalWorkItems[0] << totalWorkItems[1];

    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(),
                _kernel, 2, 0, totalWorkItems, NULL, 0, NULL, NULL));

    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 1,
            &_texture, 0, NULL, NULL));

    _mainWidget->updateGL();

    releaseCL();

}
