#ifndef MANDELBROT_H
#define MANDELBROT_H

#include "../base_benchmark.h"
#include "mandelbrot_main_widget.h"

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


class Mandelbrot
    : public BaseBenchmark
{
    Q_OBJECT

    public:
        Mandelbrot(Environment *environment, QWidget *parent = 0);
        ~Mandelbrot();

        QWidget *getConfigWidget() { return _configWidget; }
        QWidget *getMainWidget() { return _mainWidget; }
        bool waitForStop() { return true; }
        static QString getName() { return QString("Mandelbrot"); }

    public slots:
        void execute();
        void stop();
        void calculate();


    private:
        void initCL();
        void releaseCL();

        bool _isRunning;

        cl_kernel _kernel;
        MandelbrotMainWidget *_mainWidget;
        QWidget *_configWidget;

        cl_mem _texture;
};

#endif // MANDELBROT_H
