#include "readwrite.h"

ReadWrite::ReadWrite(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new QWidget(parent);
    _configWidget = new ReadWriteConfigWidget(parent);
    _source = new VideoSource();
    _vLayout = new QVBoxLayout();
    _vLayout->addWidget(_glWidget = new GlWidget());
    _vLayout->addWidget(_glWidget2 = new GlWidget());
    _mainWidget->setLayout(_vLayout);

    connect(_source, SIGNAL(frame(IplImage*)), _glWidget, SLOT(newFrame(IplImage*)));
    connect(_configWidget, SIGNAL(changedevice(bool)), _source, SLOT(changeDevice(bool)));
    connect(_configWidget, SIGNAL(fileName(QString)), _source, SLOT(fileName(QString)));
}

ReadWrite::~ReadWrite()
{

}

void ReadWrite::execute()
{
    _source->startCamera();
}

void ReadWrite::stop()
{
    _source->stopCamera();
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
