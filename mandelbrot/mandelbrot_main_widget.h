#ifndef MANDELBROT_MAIN_WIDGET_H
#define MANDELBROT_MAIN_WIDGET_H

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QVector>
#include <QVector2D>

class MandelbrotMainWidget
    : public QGLWidget
{
    Q_OBJECT

    public:
        MandelbrotMainWidget(QWidget *parent = 0);
        virtual ~MandelbrotMainWidget();

        GLuint getTexture() const { return _textureId; }

    protected:
        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();

    signals:
        void sizeChanged(int width, int height);

    private:
        QGLShaderProgram *_shaderProgram;
        QVector<QVector2D> _vertexArray;

        void makeShaders();
        void makeGeometry();
        GLuint _textureId;


};

#endif // MANDELBROT_MAIN_WIDGET_H
