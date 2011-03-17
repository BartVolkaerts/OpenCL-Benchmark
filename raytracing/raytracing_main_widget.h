#ifndef RAYTRACING_MAIN_WIDGET_H
#define RAYTRACING_MAIN_WIDGET_H
#include <QSize>
#include <QMatrix2x2>
#include <QGLShaderProgram>
#include <QGLWidget>

class RaytracingMainWidget
    : public QGLWidget
{
    Q_OBJECT
    public:
        RaytracingMainWidget(QWidget *parent = 0);
        virtual ~RaytracingMainWidget();

        GLuint getTextureId() { return _textureId; }

    public slots:
        void recreateTexture(int width, int height);

    signals:
        void resolutionChanged(int width, int height);

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

    private:
        void makeShaders();
        void makeGeometry();
        void createTexture();

    private:
        QMatrix2x2 _matrix;
        QGLShaderProgram *_shaderProgram;
        QVector<QVector2D> _vertexArray;
        GLuint _textureId;

        QSize _textureSize;

};

#endif // RAYTRACING_MAIN_WIDGET_H
