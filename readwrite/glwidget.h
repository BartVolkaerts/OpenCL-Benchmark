#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QString>
#include <QGLShader>
#include <QGLShaderProgram>
#include <QVector>
#include <QVector2D>
#include <opencv/highgui.h>
#include <opencv/cv.h>

class GlWidget : public QGLWidget
{
    Q_OBJECT
public:
    GlWidget(QWidget *parent = 0);
    ~GlWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);

    void makeShaders();
    void makeGeometry();

private:
    QGLShaderProgram *_shaderProgram;
    QVector<QVector2D> _vertexArray;
    GLuint _textureId;
    IplImage *_temp;
    QMatrix2x2 _matrix;

signals:

public slots:
    void newFrame(IplImage *);
};

#endif // GLWIDGET_H
