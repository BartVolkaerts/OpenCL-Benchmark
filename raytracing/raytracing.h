#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "raytracing_main_widget.h"
#include "../base_benchmark.h"

#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>

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
        void allocateBuffers();
        void releaseBuffers();
        void renderImage();

    private:
        QWidget *_configWidget;
        RaytracingMainWidget *_mainWidget;
        bool _isRunning;
        void createGeometry();

        int _numberOfVertices;

        // OpenCL buffers:
        cl_mem _raysFromCamera;
        cl_mem _glTexture;
        cl_mem _intersectionPoints;
        cl_mem _intersectionPointNormals;
        cl_mem _surfaceIds;

        cl_mem _triangleMesh;

        // OpenCL parameters
        cl_int2 _imageSize;
        cl_float _cameraPlaneDistance;
        cl_float4 _cameraOrigin;
        cl_float4 _emisionSource;

        cl_float4 *_geometry;

        // Kernels:
        cl_kernel _createRaysKernel;
        cl_kernel _triangleIntersectionKernel;
        cl_kernel _renderImageKernel;
};

#endif // RAYTRACING_H
