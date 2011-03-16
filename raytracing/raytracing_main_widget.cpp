#include "raytracing_main_widget.h"

Raytracing::Raytracing(QWidget *parent)
    : QGLWidget(parent)
{


}

Raytracing::~Raytracing()
{

}

void Raytracing::initializeGL()
{
    glEnable(GL_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_2D);

    makeGeometry();
    makeShaders();

    _shaderProgram->setUniformValue("matrix", _matrix);
    _shaderProgram->setUniformValue("texture", 0);
    _shaderProgram->setUniformValue("move", _moveVector);

    _shaderProgram->setAttributeArray("position", _vertexArray.constData());
    _shaderProgram->enableAttributeArray("position");

    glBindTexture(GL_TEXTURE_2D, _textureId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    _shaderProgram->disableAttributeArray("position");
}

void Raytracing::paintGL()
{

}

void Raytracing::resizeGL(int width, int height)
{
    recreateTexture(width, height);

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
    vertexShaderSource.append("uniform mat2 matrix;\n");
    vertexShaderSource.append("varying vec2 texcoord;\n");
    vertexShaderSource.append("void main()\n");
    vertexShaderSource.append("{\n");
    vertexShaderSource.append(
            "gl_Position = vec4(matrix * position, 0.0, 1.0);\n");
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
