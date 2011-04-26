#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

// Workaround OpenCV  ptrdiff_t error
#include "cstddef"

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <QTimer>
#include <QSize>

class VideoSource
    : public QObject
{
    Q_OBJECT

public:
    VideoSource(QObject *parent = 0);
    ~VideoSource();

    void startCamera();
    void stopCamera();
    bool getCaptureDev();
    double getFramerate();
    QSize getResolution();

private:
    CvCapture *_capture;
    IplImage *_img;
    QTimer *_timer;
    void checkForCam();

public slots:
    void newFrame();
    void changeDevice(bool);
    void fileName(QString);

signals:
    void frame(IplImage *);

};

#endif // VIDEOSOURCE_H
