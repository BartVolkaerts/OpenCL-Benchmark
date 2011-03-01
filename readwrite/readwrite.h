#ifndef READWRITE_H
#define READWRITE_H

#include "../environment.h"
#include "../base_benchmark.h"
#include "glwidget.h"
#include "videosource.h"
#include "readwriteconfigwidget.h"
#include <QLabel>
#include <QVBoxLayout>

class ReadWrite
    : public BaseBenchmark
{
    Q_OBJECT

public:
    ReadWrite(Environment *environment, QWidget *parent = 0);
    virtual ~ReadWrite();

    void execute();
    void stop();

    QWidget *getConfigWidget();
    QWidget *getMainWidget();

    static QString getName();

public slots:
    bool waitForStop() {return true;}

protected:
    void initCL();
    void releaseCL();

private:
    GlWidget *_glWidget;
    GlWidget *_glWidget2;
    VideoSource *_source;
    QVBoxLayout *_vLayout;

    QWidget *_mainWidget;
    QWidget *_configWidget;

};

#endif // READWRITE_H
