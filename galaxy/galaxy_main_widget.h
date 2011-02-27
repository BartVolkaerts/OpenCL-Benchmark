#ifndef GALAXY_MAIN_WIDGET_H
#define GALAXY_MAIN_WIDGET_H

#include <QGLWidget>
#include <QGLBuffer>
#include <QGLShader>
#include <QGLShaderProgram>
#include <QTimer>
#include <QPoint>
#include <QMouseEvent>
#include <QKeyEvent>

#include "camera.h"

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

class GalaxyMainWidget
    : public QGLWidget
{
    Q_OBJECT

    public:
        GalaxyMainWidget(QWidget *parent = 0);
        virtual ~GalaxyMainWidget();

        GLuint makeGeometry(int numberOfStars);
        void makeShaders();
        void setActive(bool active);

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        void mouseMoveEvent(QMouseEvent *event);
        void keyPressEvent(QKeyEvent *event);

    private:
        Camera *_camera;

        GLuint _vertexBuffer;
        GLuint _elementBuffer;
        QGLShaderProgram *_shaderProgram;

        QPoint _lastPosition;
        int _numberOfStars;

        bool _isActive;


};
#endif // GALAXY_MAIN_WIDGET_H
