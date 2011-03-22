#include <QLabel>

#include "raytracing.h"

Raytracing::Raytracing(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _configWidget = new QLabel("Config Widget", parent);
    _mainWidget = new RaytracingMainWidget(parent);

    _isRunning = false;

    connect(_mainWidget, SIGNAL(resolutionChanged(int, int)),
            this, SLOT(resolutionChanged(int, int)));

}

Raytracing::~Raytracing()
{

}

void Raytracing::initCL()
{
    cl_int error;
    _environment->createGLContext();
    _environment->createProgram(QStringList("raytracing/kernel.cl"));
    _kernel = _environment->getKernel("render");


    _texture = clCreateFromGLTexture2D(_environment->getContext(),
            CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
            _mainWidget->getTextureId(), &error);
    CHECK_ERR(error);
}

void Raytracing::releaseCL()
{
    clReleaseKernel(_kernel);
    clReleaseMemObject(_texture);
}

void Raytracing::execute()
{
    _isRunning = true;
    initCL();

    cl_float4 hostObject[4];
    hostObject[0].x = 400; hostObject[0].y = 100; hostObject[0].z = -200; hostObject[0].w = 0.f;
    hostObject[1].x = -100; hostObject[1].y = 100; hostObject[1].z = -100; hostObject[1].w = 0.f;
    hostObject[2].x = -150; hostObject[2].y = -100; hostObject[2].z = -100; hostObject[2].w = 0.f;
    hostObject[3].x = -200; hostObject[3].y = 50; hostObject[3].z = -50; hostObject[3].w = 0.f;

    cl_int error;
    cl_mem devObject = clCreateBuffer(_environment->getContext(),
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(cl_float4) * 4, hostObject, &error);
    CHECK_ERR(error);

    const size_t localWorkSize[2] = {64, 64};
    const size_t totalWorkItems[2] = {
        (_textureSize.x / localWorkSize[0] + 1) * localWorkSize[0],
        (_textureSize.y / localWorkSize[1] + 1) * localWorkSize[1]
    };

    qDebug() << totalWorkItems[0] << totalWorkItems[1];

    cl_float4 audioSource;
    int numberOfVertices = 4;

    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_texture));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_int2), &_textureSize));
    CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(cl_mem), &devObject));
    CHECK_ERR(clSetKernelArg(_kernel, 3, sizeof(cl_float4), &audioSource));
    CHECK_ERR(clSetKernelArg(_kernel, 4, sizeof(cl_int), &numberOfVertices));

    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 1,
                &_texture, 0, NULL, NULL));

    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(),
                _kernel, 2, 0, totalWorkItems, NULL, 0, NULL, NULL));
    CHECK_ERR(clFinish(_environment->getCommandQueue()));

    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 1,
                &_texture, 0, NULL, NULL));
}

void Raytracing::stop()
{
    _isRunning = false;
    releaseCL();
}

void Raytracing::resolutionChanged(int width, int height)
{
    if (_isRunning)
    {
        cl_int error;
        CHECK_ERR(clReleaseMemObject(_texture));
        _texture = clCreateFromGLTexture2D(_environment->getContext(),
                CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
                _mainWidget->getTextureId(), &error);
        CHECK_ERR(error);
    }
    _textureSize.x = width;
    _textureSize.y = height;
}

QWidget *Raytracing::getConfigWidget()
{
    return _configWidget;
}

QWidget *Raytracing::getMainWidget()
{
    return _mainWidget;
}
