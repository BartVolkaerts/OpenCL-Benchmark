#ifndef MANDELBROT_MAIN_WIDGET_H
#define MANDELBROT_MAIN_WIDGET_H

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QVector>
#include <QVector2D>
#include <QWheelEvent>
#include <QKeyEvent>

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
        void wheelEvent(QWheelEvent *event);
        void keyPressEvent(QKeyEvent *event);

    signals:
        void sizeChanged(int width, int height);
        void zoomIn(int amount);
        void zoomOut(int amount);
        void keyMove(int x, int y);

    private:
        QGLShaderProgram *_shaderProgram;
        QVector<QVector2D> _vertexArray;

        void makeShaders();
        void makeGeometry();
        GLuint _textureId;


};

#endif // MANDELBROT_MAIN_WIDGET_H
