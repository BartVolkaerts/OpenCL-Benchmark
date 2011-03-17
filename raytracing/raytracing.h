#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "raytracing_main_widget.h"
#include "../base_benchmark.h"

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

class Raytracing
    : public BaseBenchmark
{
    Q_OBJECT

    public:
        Raytracing(Environment *environment, QWidget *parent = 0);
        virtual ~Raytracing();

        QWidget *getConfigWidget();
        QWidget *getMainWidget();

        static QString getName() { return QString("Raytracing"); }
        bool waitForStop() { return false; }

    public slots:
        void execute();
        void stop();

    private slots:
        void resolutionChanged(int width, int height);

    private:
        void initCL();
        void releaseCL();

    private:
        QWidget *_configWidget;
        RaytracingMainWidget *_mainWidget;
        bool _isRunning;

        cl_int2 _textureSize;
        cl_mem _texture;
        cl_kernel _kernel;
};

#endif // RAYTRACING_H
