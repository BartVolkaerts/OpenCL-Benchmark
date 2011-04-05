#include "mandelbrot.h"
#include <QColor>
#include <QImage>
#include <QLabel>

Mandelbrot::Mandelbrot(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _texture = NULL;
    _kernel = NULL;
    _mainWidget = new MandelbrotMainWidget(parent);
    _configWidget = new MandelbrotConfigWidget(parent);

    _configWidget->setResolution(_mainWidget->width(), _mainWidget->height());
    _configWidget->supportDouble(_environment->deviceSupportsDouble());

    setStartingBoundaries();
    _maxIterations = _configWidget->getMaxIterations();
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
    connect(_configWidget, SIGNAL(maxIterationsChanged(int)),
            this, SLOT(setMaxIterations(int)));
    connect(_configWidget, SIGNAL(recalculate()),
            this, SLOT(calculate()));
    connect(_configWidget, SIGNAL(setFixedSize(bool, int, int)),
            _mainWidget, SLOT(setFixedSize(bool, int, int)));
}

Mandelbrot::~Mandelbrot()
{
    releaseCL();
}

void Mandelbrot::setStartingBoundaries()
{
    _minReal = -2.f;
    _maxReal = 1.f;
    _minImaginary = -1.2f;
}

void Mandelbrot::initCL()
{
    setStartingBoundaries();
    cl_int error;
    _environment->createGLContext();

    if (_configWidget->useDouble())
        _environment->createProgram(QStringList("mandelbrot/kernel.cl"),
                "-D USE_DOUBLE");
    else
        _environment->createProgram(QStringList("mandelbrot/kernel.cl"));

    _kernel = _environment->getKernel("calculate");

    _mainWidget->recreateTexture(_mainWidget->width(), _mainWidget->height());
    _texture = clCreateFromGLTexture2D(_environment->getContext(),
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            _mainWidget->getTexture(), &error);
    CHECK_ERR(error);
}

void Mandelbrot::releaseCL()
{
    _isRunning = false;

    if (_texture)
    {
        CHECK_ERR(clReleaseMemObject(_texture));
        _texture = NULL;
    }

    if (_kernel)
    {
        CHECK_ERR(clReleaseKernel(_kernel));
        _kernel = NULL;
    }
}

void Mandelbrot::bufferSizeChanged()
{
    if (!_isRunning)
        return;

    cl_int error;
    if (_texture)
        CHECK_ERR(clReleaseMemObject(_texture));


    _texture = clCreateFromGLTexture2D(_environment->getContext(),
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            _mainWidget->getTexture(), &error);
    CHECK_ERR(error);

    _configWidget->setResolution(_mainWidget->width(), _mainWidget->height());

    calculate();
}

void Mandelbrot::calculate()
{
    if (!_isRunning)
        return;

    if (_configWidget->useCpu())
    {
        calculateCPU();
        return;
    }
    if (_configWidget->useOpenMP())
    {
        calculateOpenMP();
        return;
    }
    cl_int error;
    _texture = clCreateFromGLTexture2D(_environment->getContext(),
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            _mainWidget->getTexture(), &error);
    CHECK_ERR(error);

    cl_int2 size;
    QApplication::setOverrideCursor(Qt::WaitCursor);

    size.s[0] = _mainWidget->width();
    size.s[1] = _mainWidget->height();

    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_texture));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_int2), &size));
    CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(cl_int), &_maxIterations));

    if (_configWidget->useDouble())
    {
        CHECK_ERR(clSetKernelArg(_kernel, 3, sizeof(cl_double), &_minReal));
        CHECK_ERR(clSetKernelArg(_kernel, 4, sizeof(cl_double), &_maxReal));
        CHECK_ERR(clSetKernelArg(_kernel, 5, sizeof(cl_double), &_minImaginary));
    }
    else
    {
        cl_float minReal = (float)_minReal;
        cl_float maxReal = (float)_maxReal;
        cl_float minImaginary = (float)_minImaginary;

        CHECK_ERR(clSetKernelArg(_kernel, 3, sizeof(cl_float), &minReal));
        CHECK_ERR(clSetKernelArg(_kernel, 4, sizeof(cl_float), &maxReal));
        CHECK_ERR(clSetKernelArg(_kernel, 5, sizeof(cl_float), &minImaginary));
    }


    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 1,
                &_texture, 0, NULL, NULL));

    const size_t localWorkSize[2] = {64, 64};
    const size_t totalWorkItems[2] = {
        (_mainWidget->width() / localWorkSize[0] + 1) * localWorkSize[0],
        (_mainWidget->height() / localWorkSize[1] + 1) * localWorkSize[1]
    };

    startTimeMeasure();
    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(),
                _kernel, 2, 0, totalWorkItems, NULL, 0, NULL, NULL));
    clFinish(_environment->getCommandQueue());
    stopTimeMeasure();

    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 1,
            &_texture, 0, NULL, NULL));

    _mainWidget->updateGL();
    _configWidget->setRenderTimeOpenCL(getTimeMeasureResults());
    QApplication::restoreOverrideCursor();
}

void Mandelbrot::calculateOpenMP()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage image(_mainWidget->width(), _mainWidget->height(), QImage::Format_RGB32);

    startTimeMeasure();
    #pragma omp parallel for
    for (int y = 0; y < _mainWidget->height(); ++y)
        for (int x = 0; x < _mainWidget->width(); ++x)
        {
            double maxImaginary = (double)(_minImaginary + (_maxReal - _minReal) *
                    _mainWidget->height() / _mainWidget->width());
            double imaginaryNumber =
                (double)(maxImaginary -
                (y) * ((maxImaginary - _minImaginary)
                    / (double)((_mainWidget->height()) - 1)));
            double realNumber = (double)(_minReal +
                    x * ((_maxReal - _minReal) / (double)_mainWidget->width()));

            double zReal, zImaginary, tmp;
            double zRealSquared, zImaginarySquared;
            int color;
            bool isInside = true;

            zReal = realNumber;
            zImaginary = imaginaryNumber;

            for(int i = 0; i < _maxIterations; ++i)
            {
                zRealSquared = zReal * zReal;
                zImaginarySquared = zImaginary * zImaginary;
                if (zRealSquared + zImaginarySquared > 4)
                {
                    isInside = false;
                    color = (i * 255 * 2) / _maxIterations;
                    break;
                }
                tmp = zRealSquared - zImaginarySquared + realNumber;
                zImaginary = 2 * zReal * zImaginary + imaginaryNumber;
                zReal = tmp;
            }

            if (isInside)
            {
                image.setPixel(x, y, Qt::black);
            }
            else
            {
                QColor renderColor;
                if (color < 255)
                    renderColor.setRed(-color + 255);
                // Blue
                if (color > 255)
                    renderColor.setGreen((-color + 255) + 255);
                // Green
                if (color > 127 && color < 255 + 127)
                    renderColor.setBlue((-color + 127) + 255);
                image.setPixel(x, y, renderColor.rgb());
            }
    }
    stopTimeMeasure();
    _configWidget->setRenderTimeOpenMP(getTimeMeasureResults());

    _mainWidget->setTexture(&image);

    QApplication::restoreOverrideCursor();
}

void Mandelbrot::calculateCPU()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage image(_mainWidget->width(), _mainWidget->height(), QImage::Format_RGB32);

    startTimeMeasure();
    for (int y = 0; y < _mainWidget->height(); ++y)
        for (int x = 0; x < _mainWidget->width(); ++x)
        {
            double maxImaginary = (double)(_minImaginary + (_maxReal - _minReal) *
                    _mainWidget->height() / _mainWidget->width());
            double imaginaryNumber =
                (double)(maxImaginary -
                (y) * ((maxImaginary - _minImaginary)
                    / (double)((_mainWidget->height()) - 1)));
            double realNumber = (double)(_minReal +
                    x * ((_maxReal - _minReal) / (double)_mainWidget->width()));

            double zReal, zImaginary, tmp;
            double zRealSquared, zImaginarySquared;
            int color;
            bool isInside = true;

            zReal = realNumber;
            zImaginary = imaginaryNumber;

            for(int i = 0; i < _maxIterations; ++i)
            {
                zRealSquared = zReal * zReal;
                zImaginarySquared = zImaginary * zImaginary;
                if (zRealSquared + zImaginarySquared > 4)
                {
                    isInside = false;
                    color = (i * 255 * 2) / _maxIterations;
                    break;
                }
                tmp = zRealSquared - zImaginarySquared + realNumber;
                zImaginary = 2 * zReal * zImaginary + imaginaryNumber;
                zReal = tmp;
            }

            if (isInside)
            {
                image.setPixel(x, y, Qt::black);
            }
            else
            {
                QColor renderColor;
                if (color < 255)
                    renderColor.setRed(-color + 255);
                // Blue
                if (color > 255)
                    renderColor.setGreen((-color + 255) + 255);
                // Green
                if (color > 127 && color < 255 + 127)
                    renderColor.setBlue((-color + 127) + 255);
                image.setPixel(x, y, renderColor.rgb());
            }
    }
    stopTimeMeasure();
    _configWidget->setRenderTimeCPU(getTimeMeasureResults());

    _mainWidget->setTexture(&image);

    QApplication::restoreOverrideCursor();
}


void Mandelbrot::execute()
{
    _isRunning = true;
    _configWidget->setRunning(true);
    _configWidget->setResolution(_mainWidget->width(), _mainWidget->height());
    initCL();
    calculate();
}

void Mandelbrot::stop()
{
    releaseCL();
    _isRunning = false;
    _configWidget->setRunning(false);
}

void Mandelbrot::zoomIn(int)
{
    //_maxIterations *= 2;
    double realRange = _maxReal - _minReal;
    double imaginaryRange = realRange *
        ((double)_mainWidget->width() / (double)_mainWidget->height());
    realRange /= 2;
    imaginaryRange /= 2;

    _minReal += realRange / 2.f;
    _maxReal -= realRange / 2.f;

    _minImaginary += imaginaryRange / 4.f;

    calculate();
}

void Mandelbrot::zoomOut(int)
{
    double realRange = _maxReal - _minReal;
    double imaginaryRange = realRange *
        ((double)_mainWidget->width() / (double)_mainWidget->height());
    realRange /= 2;
    imaginaryRange /= 2;

    _minReal -= realRange / 2.f;
    _maxReal += realRange / 2.f;
    _minImaginary -= imaginaryRange / 4.f;
    calculate();
}

void Mandelbrot::keyMove(int x, int y)
{
    if (x)
    {
        _minReal += ((double)_maxIterations / 100.f) / x;
        _maxReal += ((double)_maxIterations / 100.f) / x;
    }
    if (y)
        _minImaginary += ((double)_maxIterations /400.f) / y;
    qDebug() << "Move:" << x << ", " << y;
    calculate();

}

void Mandelbrot::translate(double dX, double dY)
{
    double xScale = _maxReal - _minReal;
    double yScale = xScale *
        ((double)_mainWidget->height() / (double)_mainWidget->width());

    _minReal += xScale * dX;
    _maxReal += xScale * dX;
    _minImaginary -= yScale * dY;
    calculate();
}

void Mandelbrot::setMaxIterations(int iterations)
{
    _maxIterations = iterations;
}
