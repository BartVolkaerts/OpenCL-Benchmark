#include <QImage>
#include <QVector>

#include "mandelbrot_main_widget.h"

MandelbrotMainWidget::MandelbrotMainWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    _matrix.setToIdentity();
    _moveVector.setX(0.0);
    _moveVector.setY(0.0);
}

MandelbrotMainWidget::~MandelbrotMainWidget()
{
}

void MandelbrotMainWidget::paintGL()
{
    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    _shaderProgram->setUniformValue("matrix", _matrix);
    _shaderProgram->setUniformValue("texture", 0);
    _shaderProgram->setUniformValue("move", _moveVector);

    _shaderProgram->setAttributeArray("position", _vertexArray.constData());
    _shaderProgram->enableAttributeArray("position");

    glBindTexture(GL_TEXTURE_2D, _textureId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    _shaderProgram->disableAttributeArray("position");
}

void MandelbrotMainWidget::initializeGL()
{
    glEnable(GL_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_2D);

    makeShaders();
    makeGeometry();


    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // GL_RGBA -> 4 floats per kleur -> CL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width(), height(), 0, GL_RGBA,
            GL_UNSIGNED_BYTE, 0);


}

void MandelbrotMainWidget::resizeGL(int width, int height)
{
    glDeleteTextures(1, &_textureId);

    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT, 0);

    glViewport(0, 0, width, height);
    emit sizeChanged(width, height);
}

void MandelbrotMainWidget::makeGeometry()
{
    _vertexArray.append(QVector2D(-1.0f, -1.0f));
    _vertexArray.append(QVector2D(1.0f, -1.0f));
    _vertexArray.append(QVector2D(-1.0f, 1.0f));
    _vertexArray.append(QVector2D(1.0f, 1.0f));
}

void MandelbrotMainWidget::makeShaders()
{
    _shaderProgram = new QGLShaderProgram(this);
    QGLShader *vertexShader = new QGLShader(QGLShader::Vertex, this);
    QGLShader *fragmentShader = new QGLShader(QGLShader::Fragment, this);

    QString vertexShaderSource;
    QString fragmentShaderSource;

    vertexShaderSource.append("#version 110\n");
    vertexShaderSource.append("attribute vec2 position;\n");
    vertexShaderSource.append("uniform vec2 move;\n");
    vertexShaderSource.append("uniform mat2 matrix;\n");
    vertexShaderSource.append("varying vec2 texcoord;\n");
    vertexShaderSource.append("void main()\n");
    vertexShaderSource.append("{\n");
    vertexShaderSource.append(
            "gl_Position = vec4(matrix * (position + move), 0.0, 1.0);\n");
    vertexShaderSource.append("texcoord = vec2(position.x, -position.y) "
            "* vec2(0.5) + vec2(0.5);\n");
    vertexShaderSource.append("}\n");

    fragmentShaderSource.append("#version 110\n");
    fragmentShaderSource.append("uniform sampler2D texture;\n");
    fragmentShaderSource.append("varying vec2 texcoord;\n");
    fragmentShaderSource.append("void main()\n");
    fragmentShaderSource.append("{\n");
    fragmentShaderSource.append(
            "gl_FragColor = texture2D(texture, texcoord);\n");
    fragmentShaderSource.append("}\n");

    vertexShader->compileSourceCode(vertexShaderSource);
    fragmentShader->compileSourceCode(fragmentShaderSource);

    if (!vertexShader->isCompiled())
    {
        qDebug()
            << "Could not compile vertex shader:\n"
            << vertexShader->log();
    }

    if (!fragmentShader->isCompiled())
    {
        qDebug()
            << "Could not compile fragment shader:\n"
            << fragmentShader->log();
    }

    _shaderProgram->addShader(fragmentShader);
    _shaderProgram->addShader(vertexShader);

    _shaderProgram->link();
    _shaderProgram->bind();

}

void MandelbrotMainWidget::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0)
    {
        emit zoomIn(event->delta());
    }
    if (event->delta() < 0)
    {
        emit zoomOut(- event->delta());
    }
}

void MandelbrotMainWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
            emit keyMove(0, -100);
            break;
        case Qt::Key_Down:
            emit keyMove(0, 100);
            break;
        case Qt::Key_Left:
            emit keyMove(100, 0);
            break;
        case Qt::Key_Right:
            emit keyMove(-100, 0);
            break;
    }
}

void MandelbrotMainWidget::translate(const QPoint &point)
{
    QVector2D vector;
    vector.setX(- ((double)point.x() / (double)width()));
    vector.setY(((double)point.y() / (double)height()));

    _moveVector += vector;

    updateGL();
}

void MandelbrotMainWidget::resetTranslation()
{
    _moveVector.setX(0.0);
    _moveVector.setY(0.0);
}

void MandelbrotMainWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
    {
        _prevMousePos = event->pos();
        event->ignore();
        return;
    }

    translate((_prevMousePos - event->pos()) * 2.0);
    _prevMousePos = event->pos();
}

void MandelbrotMainWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _prevMousePos = event->pos();
        _lastRenderPos = event->pos();
    }
    else
    {
        event->ignore();
    }
}

void MandelbrotMainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!(event->button() & Qt::LeftButton))
    {
        event->ignore();
        return;
    }
    emit positionChanged(
        (double)(_lastRenderPos.x() - event->pos().x()) / (double)width(),
        (double)(_lastRenderPos.y() - event->pos().y()) / (double)height());
    resetTranslation();
    updateGL();
}

void MandelbrotMainWidget::setFixedSize(bool isFixed, int width, int height)
{
    if (isFixed)
    {
        this->setMinimumHeight(height);
        this->setMaximumHeight(height);
        this->setMinimumWidth(width);
        this->setMaximumWidth(width);
    }
    else
    {
        this->setMinimumHeight(0);
        this->setMaximumHeight(QWIDGETSIZE_MAX);
        this->setMinimumWidth(0);
        this->setMaximumWidth(QWIDGETSIZE_MAX);
    }
}
