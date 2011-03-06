#ifndef READWRITE_H
#define READWRITE_H

#include "../environment.h"
#include "../base_benchmark.h"
#include "glwidget.h"
#include "videosource.h"
#include "readwriteconfigwidget.h"
#include <QLabel>
#include <QVBoxLayout>

#ifdef __APPLE__
#   include <OpenCL/opencl.h>
#else
#   include <CL/opencl.h>
#endif

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
    VideoSource *_source;
    QVBoxLayout *_vLayout;

    QWidget *_mainWidget;
    QWidget *_configWidget;

    Environment *_environment;

    cl_kernel _kernel;
    cl_mem _image;

};

#endif // READWRITE_H
