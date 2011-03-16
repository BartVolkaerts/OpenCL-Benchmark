#ifndef RAYTRACING_MAIN_WIDGET_H
#define RAYTRACING_MAIN_WIDGET_H
#include <QGLWidget>

class RaytracingMainWidget
    : public QGLWidget
{
    Q_OBJECT
    public:
        RaytracingMainWidget(QWidget *parent = 0);
        virtual ~RaytracingMainWidget();

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

    signals:
        void sizeChanged(int width, int height);

    private:
        QMatrix2x2 _matrix;

        void makeShaders();
        void makeGeometry();

        QGLShaderProgram *_shaderProgram;
        QVector<QVector2D> _vertexArray;

        GLuint _textureId;

};

#endif // RAYTRACING_MAIN_WIDGET_H
