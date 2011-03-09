#include "mandelbrot.h"
#include <QLabel>

Mandelbrot::Mandelbrot(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new MandelbrotMainWidget(parent);
    _configWidget = new QLabel("Mandelbrot", parent);

    _maxIterations = 300;
    _minReal = -2.f;
    _maxReal = 1.f;
    _minImaginary = -1.2f;
    _isRunning = false;

    connect(_mainWidget, SIGNAL(sizeChanged(int, int)),
            this, SLOT(bufferSizeChanged()));
    connect(_mainWidget, SIGNAL(zoomIn(int)),
            this, SLOT(zoomIn(int)));
    connect(_mainWidget, SIGNAL(zoomOut(int)),
            this, SLOT(zoomOut(int)));
    connect(_mainWidget, SIGNAL(keyMove(int, int)),
            this, SLOT(keyMove(int, int)));
    connect(_mainWidget, SIGNAL(positionChanged(double, double)),
            this, SLOT(translate(double, double)));
}

Mandelbrot::~Mandelbrot()
{
}

void Mandelbrot::initCL()
{
    cl_int error;
    _environment->createGLContext();
    qDebug() << _environment->getDeviceExtensions();
    _environment->createProgram(QStringList("mandelbrot/kernel.cl"));
    _kernel = _environment->getKernel("calculate");

    _texture = clCreateFromGLTexture2D(_environment->getContext(),
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            _mainWidget->getTexture(), &error);
    CHECK_ERR(error);
}

void Mandelbrot::releaseCL()
{
    //CHECK_ERR(clReleaseMemObject(_texture));
}

void Mandelbrot::bufferSizeChanged()
{
    calculate();
}

void Mandelbrot::calculate()
{
    if (!_isRunning)
        return;

    cl_int2 size;
    cl_int error;
    cl_int maxIterations = 200;
    cl_float minReal = -1.f;//-2.f;
    cl_float maxReal = 0.5f;//1.f;
    cl_float minImaginary = -.6f;//-1.2f;

    // CL Image from GL texture
#if 0
    _texture = clCreateFromGLTexture2D(_environment->getContext(),
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            _mainWidget->getTexture(), &error);
    CHECK_ERR(error);
#endif

    size.x = _mainWidget->width();
    size.y = _mainWidget->height();

    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_texture));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_int2), &size));
    CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(cl_int), &_maxIterations));
    CHECK_ERR(clSetKernelArg(_kernel, 3, sizeof(cl_float), &_minReal));
    CHECK_ERR(clSetKernelArg(_kernel, 4, sizeof(cl_float), &_maxReal));
    CHECK_ERR(clSetKernelArg(_kernel, 5, sizeof(cl_float), &_minImaginary));


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
}

void Mandelbrot::execute()
{
    _isRunning = true;
    initCL();

    calculate();
}

void Mandelbrot::stop()
{
    releaseCL();
    _isRunning = false;
}

void Mandelbrot::zoomIn(int amount)
{
    //_maxIterations *= 2;
    double realRange = _maxReal - _minReal;
    double imaginaryRange = realRange *
        ((double)_mainWidget->width() / (double)_mainWidget->height());
    realRange /= 2;
    imaginaryRange /= 2;

    _minReal += realRange / 2.f;
    _maxReal -= realRange / 2.f;

    _minImaginary += imaginaryRange / 2.f;

    calculate();
}

void Mandelbrot::zoomOut(int amount)
{
    double realRange = _maxReal - _minReal;
    double imaginaryRange = realRange *
        ((double)_mainWidget->width() / (double)_mainWidget->height());
    realRange /= 2;
    imaginaryRange /= 2;

    _minReal -= realRange / 2.f;
    _maxReal += realRange / 2.f;
    _minImaginary -= imaginaryRange / 2.f;
    calculate();
}

void Mandelbrot::keyMove(int x, int y)
{
    if (x)
    {
        _minReal += ((float)_maxIterations / 100.f) / x;
        _maxReal += ((float)_maxIterations / 100.f) / x;
    }
    if (y)
        _minImaginary += ((float)_maxIterations /400.f) / y;
    qDebug() << "Move:" << x << ", " << y;
    calculate();

}

void Mandelbrot::translate(double dX, double dY)
{
    double xScale = _maxReal - _minReal;
    double yScale = xScale *
        ((double)_mainWidget->width() / (double)_mainWidget->height());

    _minReal += xScale * dX;
    _maxReal += xScale * dX;
    _minImaginary -= yScale * dY;
    calculate();
}
