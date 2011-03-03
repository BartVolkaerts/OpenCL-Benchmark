#include "mandelbrot_main_widget.h"

MandelbrotMainWidget::MandelbrotMainWidget(QWidget *parent)
    : QGLWidget(parent)
{

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

    _shaderProgram->setUniformValue("texture", 0);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // GL_RGBA32UI -> 32 bit per kleur -> CL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, width(), height(), 0, GL_RGBA, GL_UNSIGNED_INT, 0);


}

void MandelbrotMainWidget::resizeGL(int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, width, height, 0, GL_RGBA, GL_UNSIGNED_INT, 0);

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
    vertexShaderSource.append("//uniform mat2 matrix;\n");
    vertexShaderSource.append("varying vec2 texcoord;\n");
    vertexShaderSource.append("void main()\n");
    vertexShaderSource.append("{\n");
    vertexShaderSource.append(
            "gl_Position = vec4(/*matrix */ position, 0.0, 1.0);\n");
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
