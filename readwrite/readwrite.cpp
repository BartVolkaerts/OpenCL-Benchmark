#include "readwrite.h"

ReadWrite::ReadWrite(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new QWidget(parent);
    _configWidget = new ReadWriteConfigWidget(parent);
    _source = new VideoSource();
    _vLayout = new QVBoxLayout();
    _vLayout->addWidget(_glWidget = new GlWidget());
    _mainWidget->setLayout(_vLayout);

    _kernelByteToFloat = NULL;

    connect(_source, SIGNAL(frame(IplImage*)), this, SLOT(newFrame(IplImage*)));
    connect(_configWidget, SIGNAL(changedevice(bool)), _source, SLOT(changeDevice(bool)));
    connect(_configWidget, SIGNAL(fileName(QString)), _source, SLOT(fileName(QString)));

}

ReadWrite::~ReadWrite()
{

}

void ReadWrite::newFrame(IplImage *image)
{
    cl_int error;
    uchar *data = (uchar *)image->imageData;
    uchar *temp;

    _input = clCreateBuffer(_environment->getContext(), CL_MEM_READ_WRITE,
                            sizeof(cl_uchar)*image->imageSize, data, &error);
    _temp = clCreateBuffer(_environment->getContext(), CL_MEM_READ_WRITE,
                            sizeof(cl_uchar)*image->imageSize, NULL, &error);
    CHECK_ERR(error);

    CHECK_ERR(clSetKernelArg(_kernelByteToFloat, 0, sizeof(cl_mem), &_input));
    CHECK_ERR(clSetKernelArg(_kernelByteToFloat, 1, sizeof(cl_mem), &_temp));

    const size_t localWorkSize[2] = {64, 64};
    const size_t totalWorkItems[2] = {
        (image->width / localWorkSize[0] + 1) * localWorkSize[0],
        (image->height / localWorkSize[1] + 1) * localWorkSize[1]
    };
    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(), _kernelByteToFloat,
                                     1, NULL, totalWorkItems, NULL, 0, NULL, NULL));


    CHECK_ERR(clEnqueueReadBuffer(_environment->getCommandQueue(), _temp, CL_TRUE,
                                  0, sizeof(uchar)*image->imageSize, temp, 0, NULL,
                                  NULL));

    clReleaseMemObject(_input);

    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 1,
                &_image, 0, NULL, NULL));

    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 1,
            &_image, 0, NULL, NULL));

    _glWidget->newFrame(image);
}

void ReadWrite::execute()
{
    initCL();
    _source->startCamera();
}

void ReadWrite::stop()
{
    _source->stopCamera();
    releaseCL();
}

void ReadWrite::initCL()
{
    cl_int error;

    _environment->createGLContext();

    _environment->createProgram(QStringList("readwrite/kernel.cl"));
    _kernelByteToFloat = _environment->getKernel("byteToFloat");

    _image = clCreateFromGLTexture2D(_environment->getContext(),
                                     CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0,
                                     _glWidget->getTexture(), &error);
    CHECK_ERR(error);
}

void ReadWrite::releaseCL()
{
    clReleaseKernel(_kernelByteToFloat);

    _environment->createContext();
}

QWidget *ReadWrite::getConfigWidget()
{
    return _configWidget;
}

QWidget *ReadWrite::getMainWidget()
{
    return _mainWidget;
}

QString ReadWrite::getName()
{
    return QString("Image processing");
}
