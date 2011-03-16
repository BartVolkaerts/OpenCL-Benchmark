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

    GLuint getLeftTexture() { return _inputTextureId; }
    GLuint getRightTexture() { return _outputTextureId; }

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int, int);
    void createLeftTexture();
    void createRightTexture();
    void recreateTextures();
    void updateLeftTexture();
    void updateRightTexture();

    void makeShaders();
    void makeGeometry();

private:
    QGLShaderProgram *_shaderProgram;
    QVector<QVector2D> _leftVertexArray;
    QVector<QVector2D> _rightVertexArray;
    GLuint _inputTextureId;
    GLuint _outputTextureId;
    IplImage *_temp;
    QMatrix2x2 _matrix;

signals:

public slots:
    void newFrame(IplImage *);
};

#endif // GLWIDGET_H
