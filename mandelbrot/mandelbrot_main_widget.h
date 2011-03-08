#ifndef MANDELBROT_MAIN_WIDGET_H
#define MANDELBROT_MAIN_WIDGET_H

#include <QGLWidget>
#include <QGLShaderProgram>
#include <QVector>
#include <QVector2D>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QMatrix2x2>
#include <QMouseEvent>
#include <QPoint>

class MandelbrotMainWidget
    : public QGLWidget
{
    Q_OBJECT

    public:
        MandelbrotMainWidget(QWidget *parent = 0);
        virtual ~MandelbrotMainWidget();

        GLuint getTexture() const { return _textureId; }
        void translate(const QPoint &point);
        void resetTranslation();

    protected:
        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();
        void wheelEvent(QWheelEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);

    signals:
        void sizeChanged(int width, int height);
        void zoomIn(int amount);
        void zoomOut(int amount);
        void keyMove(int x, int y);
        void positionChanged(double dX, double dY);

    private:
        QGLShaderProgram *_shaderProgram;
        QVector<QVector2D> _vertexArray;

        void makeShaders();
        void makeGeometry();
        GLuint _textureId;

        QMatrix2x2 _matrix;
        QVector2D _moveVector;

        QPoint _prevMousePos;
        QPoint _lastRenderPos;




};

#endif // MANDELBROT_MAIN_WIDGET_H
