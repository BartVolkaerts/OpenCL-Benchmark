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

    _kernel = NULL;

    connect(_source, SIGNAL(frame(IplImage*)), this, SLOT(newFrame(IplImage*)));
    connect(_configWidget, SIGNAL(changedevice(bool)), _source, SLOT(changeDevice(bool)));
    connect(_configWidget, SIGNAL(fileName(QString)), _source, SLOT(fileName(QString)));
    connect(this, SIGNAL(stopRunning(bool)), parent, SLOT(stopBenchmark()));
}

ReadWrite::~ReadWrite()
{

}

void ReadWrite::newFrame(IplImage *image)
{
    cl_int error;
    cl_int2 size;

    _glWidget->newFrame(image);
    glFlush();

    //GPU part
    size.x = image->width;
    size.y = image->height;

    _input = clCreateFromGLTexture2D(_environment->getContext(), CL_MEM_READ_WRITE,
                                     GL_TEXTURE_2D, 0, _glWidget->getLeftTexture(), &error);
    CHECK_ERR(error);

    _output = clCreateFromGLTexture2D(_environment->getContext(), CL_MEM_READ_WRITE,
                                     GL_TEXTURE_2D, 0, _glWidget->getRightTexture(), &error);
    CHECK_ERR(error);

    cl_mem texturesArray[] = {_input, _output};

    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_input));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_mem), &_output));
    CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(cl_int2), &size));

    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 2,
                                        texturesArray, 0, NULL, NULL));

    const size_t localWorkSize[2] = {64, 64};
    const size_t totalWorkItems[2] = {
        (size.x / localWorkSize[0] + 1) * localWorkSize[0],
        (size.y / localWorkSize[1] + 1) * localWorkSize[1]
    };

    startTimeMeasure();
    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(),
                _kernel, 2, 0, totalWorkItems, NULL, 0, NULL, NULL));
    CHECK_ERR(clFinish(_environment->getCommandQueue()));
    stopTimeMeasure();

    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 2,
            texturesArray, 0, NULL, NULL));

    _configWidget->setGPUTime(getTimeMeasureResults());

    CHECK_ERR(clReleaseMemObject(_input));
    CHECK_ERR(clReleaseMemObject(_output));

    //CPU part
    startTimeMeasure();
    uchar *temp = (uchar *)image->imageData;
    cl_float4 *output = new cl_float4[(image->imageSize)/image->nChannels];
    for (int i = 0; i < image->imageSize; i+=image->nChannels)
    {
        output[i/image->nChannels].x = (float)temp[i]/255.f;
        output[i/image->nChannels].y = (float)temp[i+1]/255.f;
        output[i/image->nChannels].z = (float)temp[i+2]/255.f;
    }
    for(int i = 0; i<(image->imageSize/image->nChannels); i++)
    {
        if(output[i].x > 0.675f && output[i].x < 0.835f &&
            output[i].y > 0.526f && output[i].y < 0.706f &&
                output[i].z > 0.566f && output[i].z < 0.766f)
        {
            output[i].x = 1.0f;
            output[i].y = 1.0f;
            output[i].z = 1.0f;
        }
        else
        {
            output[i].x = 0.0f;
            output[i].y = 0.0f;
            output[i].z = 0.0f;
        }
    }
    stopTimeMeasure();
    delete output;
    _configWidget->setCPUTime(getTimeMeasureResults());

    //update textures
    _glWidget->updateGL();
}

void ReadWrite::execute()
{
    if(_source->getCaptureDev())
    {
        initCL();
        _source->startCamera();
        _configWidget->setLocked();
    }
    else
    {
        QMessageBox::information(_configWidget, "Oops..", "Please select a valid videosource.", QMessageBox::Ok, 0);
        emit stopRunning(true);
    }

}

void ReadWrite::stop()
{
    _source->stopCamera();
    releaseCL();
    _configWidget->setUnlocked();
}

void ReadWrite::initCL()
{
    _environment->createGLContext();
    _environment->createProgram(QStringList("readwrite/kernel.cl"));

    _kernel = _environment->getKernel("calculate");
}

void ReadWrite::releaseCL()
{
    clReleaseKernel(_kernel);
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
