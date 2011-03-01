#include "videosource.h"
#include <QDebug>

VideoSource::VideoSource(QObject *parent)
    : QObject(parent)
{
    _capture = NULL;
    _timer = new QTimer(this);
    _img = NULL;

    connect(this->_timer, SIGNAL(timeout()), this, SLOT(newFrame()));
}

VideoSource::~VideoSource()
{

}

void VideoSource::changeDevice(bool state)
{
    qDebug() << state;
    if (state)
    {
        if (_capture)
        {
            cvReleaseCapture(&_capture);
        }
        _capture = cvCaptureFromCAM(0);
    }
    else
    {
        if (_capture)
        {
            cvReleaseCapture(&_capture);
        }
    }
}

void VideoSource::fileName(QString name)
{
    qDebug() << name;
    if (_capture)
    {
        cvReleaseCapture(&_capture);
    }
    _capture = cvCaptureFromAVI(name.toLatin1());

}

void VideoSource::newFrame()
{
    _img = cvQueryFrame(_capture);
    emit frame(_img);
}

void VideoSource::startCamera()
{
    int frameRate = cvGetCaptureProperty(_capture, CV_CAP_PROP_FPS);
    if (frameRate == -1)
        frameRate = 30;
    _timer->start(qRound(1000.0/frameRate));
}

void VideoSource::stopCamera()
{
    _timer->stop();
}


