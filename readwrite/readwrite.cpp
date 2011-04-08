#include "readwrite.h"

ReadWrite::ReadWrite(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new GlWidget(parent);
    _configWidget = new ReadWriteConfigWidget(parent);
    _source = new VideoSource(this);

    _edgekernel = NULL;
    _sharpkernel = NULL;

    connect(_source, SIGNAL(frame(IplImage*)), this, SLOT(newFrame(IplImage*)));
    connect(_configWidget, SIGNAL(changedevice(bool)), _source, SLOT(changeDevice(bool)));
    connect(_configWidget, SIGNAL(fileName(QString)), _source, SLOT(fileName(QString)));
    connect(this, SIGNAL(stopRunning(bool)), parent, SLOT(stopBenchmark()));
}

ReadWrite::~ReadWrite()
{
    clReleaseKernel(_edgekernel);
    clReleaseKernel(_sharpkernel);
}

void ReadWrite::newFrame(IplImage *image)
{
    cl_int error;
    cl_int2 size;

    _mainWidget->newFrame(image);
    glFlush();

    //GPU part
    size.s[0] = image->width;
    size.s[1] = image->height;

    _input = clCreateFromGLTexture2D(_environment->getContext(), CL_MEM_READ_WRITE,
                                     GL_TEXTURE_2D, 0, _mainWidget->getLeftTexture(), &error);
    CHECK_ERR(error);

    _output = clCreateFromGLTexture2D(_environment->getContext(), CL_MEM_READ_WRITE,
                                     GL_TEXTURE_2D, 0, _mainWidget->getRightTexture(), &error);
    CHECK_ERR(error);

    cl_mem texturesArray[] = {_input, _output};

    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 2,
                                        texturesArray, 0, NULL, NULL));

    const size_t localWorkSize[2] = {64, 64};
    const size_t totalWorkItems[2] = {
        (size.s[0] / localWorkSize[0] + 1) * localWorkSize[0],
        (size.s[1] / localWorkSize[1] + 1) * localWorkSize[1]
    };

    if(_configWidget->copyImage())
    {
        const size_t src[3]={0,0,0};
        const size_t dst[3]={0,0,0};
        const size_t reg[3]={image->width,image->height,1};

        startTimeMeasure();
        CHECK_ERR(clEnqueueCopyImage(_environment->getCommandQueue(), _input, _output,
                                     src,dst,reg,0,NULL,NULL));
        CHECK_ERR(clFinish(_environment->getCommandQueue()));
        stopTimeMeasure();
    }
    else if(_configWidget->useEdgeDetection())
    {
        CHECK_ERR(clSetKernelArg(_edgekernel, 0, sizeof(cl_mem), &_input));
        CHECK_ERR(clSetKernelArg(_edgekernel, 1, sizeof(cl_mem), &_output));
        CHECK_ERR(clSetKernelArg(_edgekernel, 2, sizeof(cl_int2), &size));

        startTimeMeasure();
        CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(),
                    _edgekernel, 2, 0, totalWorkItems, NULL, 0, NULL, NULL));
        CHECK_ERR(clFinish(_environment->getCommandQueue()));
        stopTimeMeasure();
    }
    else if(_configWidget->useSharpening())
    {
        CHECK_ERR(clSetKernelArg(_sharpkernel, 0, sizeof(cl_mem), &_input));
        CHECK_ERR(clSetKernelArg(_sharpkernel, 1, sizeof(cl_mem), &_output));
        CHECK_ERR(clSetKernelArg(_sharpkernel, 2, sizeof(cl_int2), &size));

        startTimeMeasure();
        CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(),
                    _sharpkernel, 2, 0, totalWorkItems, NULL, 0, NULL, NULL));
        CHECK_ERR(clFinish(_environment->getCommandQueue()));
        stopTimeMeasure();
    }

    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 2,
            texturesArray, 0, NULL, NULL));



    CHECK_ERR(clReleaseMemObject(_input));
    CHECK_ERR(clReleaseMemObject(_output));

    //update textures
    _mainWidget->updateGL();
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
    _edgekernel = _environment->getKernel("edgedetect");
    _sharpkernel = _environment->getKernel("sharpening");
}

void ReadWrite::releaseCL()
{
    clReleaseKernel(_edgekernel);
    clReleaseKernel(_sharpkernel);
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
