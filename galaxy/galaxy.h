#ifndef GALAXY_H
#define GALAXY_H
//TODO: Platform independent.

#include "../base_benchmark.h"
#include "../environment.h"
#include "galaxy_main_widget.h"

#include <QTimer>
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

class Galaxy
    : public BaseBenchmark
{
    Q_OBJECT

    public:
        Galaxy(Environment *environment, QWidget *parent = 0);
        virtual ~Galaxy();

        QWidget *getConfigWidget();
        QWidget *getMainWidget();
        bool waitForStop();

        static QString getName();

    public slots:
        void execute();
        void stop();

    private:
        void initCL();
        void releaseCL();
        void generateRandom(cl_float4 *objects, int number, int boundary);
        void createInput(int numberOfStars, int numberOfGalacticCenters);

    private slots:
        void simulationStep();

    private:
        QWidget *_configWidget;
        GalaxyMainWidget *_mainWidget;
        cl_kernel _kernel;

        cl_mem _starBuffer;
        cl_mem _starSpeedBuffer;
        cl_mem _galacticCenterBuffer;

        cl_float4 *_stars;
        cl_float4 *_galacticCenters;
        cl_float4 *_starSpeed;

        GLuint _starVBO;

        QTimer *_timer;

};

#endif // GALAXY_H
