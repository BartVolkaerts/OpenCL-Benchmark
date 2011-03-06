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

    connect(_source, SIGNAL(frame(IplImage*)), _glWidget, SLOT(newFrame(IplImage*)));
    connect(_configWidget, SIGNAL(changedevice(bool)), _source, SLOT(changeDevice(bool)));
    connect(_configWidget, SIGNAL(fileName(QString)), _source, SLOT(fileName(QString)));
}

ReadWrite::~ReadWrite()
{

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
    _environment->createGLContext();
    //_image = clCreateFromGLTexture2D(_environment->getContext(),)
    _environment->createProgram(QStringList("readwrite/kernel.cl"));
    _kernel = _environment->getKernel("go");
}

void ReadWrite::releaseCL()
{
    if (_kernel)
        clReleaseKernel(_kernel);
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
    return QString("OpenCV/OpenCL");
}
