#ifndef READWRITE_H
#define READWRITE_H

#include "../environment.h"
#include "../base_benchmark.h"
#include "glwidget.h"
#include "videosource.h"
#include "readwriteconfigwidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>

#ifdef __APPLE__
#   include <OpenCL/cl_gl_ext.h>
#   include <OpenCL/cl_gl.h>
#   include <OpenGL/glx.h>
#   define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#else
#   include <CL/cl_gl_ext.h>
#   include <CL/cl_gl.h>
#   include <GL/glx.h>
#   define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
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

private slots:
    void newFrame(IplImage *);
    void setSourceProp();


protected:
    void initCL();
    void releaseCL();

private:
    GlWidget *_mainWidget;
    ReadWriteConfigWidget *_configWidget;

    VideoSource *_source;

    cl_kernel _kernel;
    cl_mem _output;
    cl_mem _input;

signals:
    void stopRunning(bool);
    void addFrame();

};

#endif // READWRITE_H
