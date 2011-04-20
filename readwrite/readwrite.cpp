#include "readwrite.h"

ReadWrite::ReadWrite(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new GlWidget();
    _configWidget = new ReadWriteConfigWidget(parent);
    _source = new VideoSource(this);

    _kernel = NULL;

    connect(_configWidget, SIGNAL(changedevice(bool)), _source, SLOT(changeDevice(bool)));
    connect(_configWidget, SIGNAL(fileName(QString)), _source, SLOT(fileName(QString)));
    connect(_source, SIGNAL(frame(IplImage*)), this, SLOT(newFrame(IplImage*)));
    connect(_configWidget, SIGNAL(changedevice(bool)), this, SLOT(setSourceProp()));
    connect(_configWidget, SIGNAL(fileName(QString)), this, SLOT(setSourceProp()));

    connect(this,SIGNAL(addFrame()), _configWidget, SLOT(addFrame()));

    connect(this, SIGNAL(stopRunning(bool)), parent, SLOT(stopBenchmark()));
}

ReadWrite::~ReadWrite()
{
    clReleaseKernel(_kernel);
}

void ReadWrite::newFrame(IplImage *image)
{
    cl_int error;
    cl_int2 size;
    cl_int check;

    _mainWidget->newFrame(image);

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
    else
    {
        if (_configWidget->useEdgeDetection())
            check = 0;
        if (_configWidget->useSharpening())
            check = 1;

        CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_input));
        CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_mem), &_output));
        CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(cl_int2), &size));
        CHECK_ERR(clSetKernelArg(_kernel, 3, sizeof(cl_int), &check));

        startTimeMeasure();
        CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(),
                    _kernel, 2, 0, totalWorkItems, NULL, 0, NULL, NULL));
        CHECK_ERR(clFinish(_environment->getCommandQueue()));
        stopTimeMeasure();
    }

    _configWidget->setOpenCLTime(getTimeMeasureResults());
    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 2,
            texturesArray, 0, NULL, NULL));

    CHECK_ERR(clReleaseMemObject(_input));
    CHECK_ERR(clReleaseMemObject(_output));

    //update textures
    _mainWidget->updateGL();

    emit addFrame();
}

void ReadWrite::execute()
{
    if(_source->getCaptureDev())
    {
        _mainWidget->updateGL();
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
    _kernel = _environment->getKernel("process");
}

void ReadWrite::releaseCL()
{
    clReleaseKernel(_kernel);
    _environment->createContext();
}

void ReadWrite::setSourceProp()
{
    QSize size = _source->getResolution();
    QString sizeProp;
    sizeProp.append(QString::number(size.width()));
    sizeProp.append(" x ");
    sizeProp.append(QString::number(size.height()));

    _configWidget->setFramerate(QString::number(_source->getFramerate()));
    _configWidget->setResolution(sizeProp);
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
