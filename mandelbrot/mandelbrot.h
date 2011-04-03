#ifndef MANDELBROT_H
#define MANDELBROT_H

#include "../base_benchmark.h"
#include "mandelbrot_main_widget.h"
#include "mandelbrot_config_widget.h"

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
        void bufferSizeChanged();
        void calculate();
        void calculateCPU();
        void calculateOpenMP();

        void zoomIn(int amount);
        void zoomOut(int amount);
        void keyMove(int x, int y);
        void translate(double dX, double dY);

        void setMaxIterations(int iterations);

    private:
        void initCL();
        void releaseCL();
        void setStartingBoundaries();

        bool _isRunning;

        cl_kernel _kernel;
        MandelbrotMainWidget *_mainWidget;
        MandelbrotConfigWidget *_configWidget;

        cl_mem _texture;

        cl_int _maxIterations;

        cl_double _minReal;
        cl_double _maxReal;
        cl_double _minImaginary;
};

#endif // MANDELBROT_H
