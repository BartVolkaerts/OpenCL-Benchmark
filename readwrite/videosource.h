#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <QTimer>

class VideoSource
    : public QObject
{
    Q_OBJECT

public:
    VideoSource(QObject *parent = 0);
    ~VideoSource();

    void startCamera();
    void stopCamera();

private:
    CvCapture *_capture;
    IplImage *_img;
    QTimer *_timer;

public slots:
    void newFrame();
    void changeDevice(bool);
    void fileName(QString);

signals:
    void frame(IplImage *);

};

#endif // VIDEOSOURCE_H