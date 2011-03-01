#include "glwidget.h"

GlWidget::GlWidget(QWidget *parent) :
    QGLWidget(parent)
{
    _temp = NULL;
}

GlWidget::~GlWidget()
{

}

void GlWidget::initializeGL()
{
    glEnable(GL_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_2D);

    makeShaders();
    makeGeometry();

    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void GlWidget::paintGL()
{
    if (!_temp)
        return;

    qglClearColor(Qt::black);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
            _temp->width, _temp->height, 0, GL_BGR,
            GL_UNSIGNED_BYTE, _temp->imageData);

    _shaderProgram->setUniformValue("texture", 0);

    _shaderProgram->setUniformValue("matrix",
            _matrix);

    _shaderProgram->setAttributeArray("position", _vertexArray.constData());
    _shaderProgram->enableAttributeArray("position");

    glBindTexture(GL_TEXTURE_2D, _textureId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    _shaderProgram->disableAttributeArray("position");
}

void GlWidget::resizeGL(int width, int height)
{
    if (!_temp)
        return;

    double projectionMatrixValues[4];
    double scaleMatrixValues[4];

    if(height * _temp->width < width * _temp->height)
    {
        projectionMatrixValues[0] = (double) height / (double)width;
        projectionMatrixValues[1] = 0.0;
        projectionMatrixValues[2] = 0.0;
        projectionMatrixValues[3] = 1.0;

        scaleMatrixValues[0] = (double)_temp->width / (double)_temp->height;
        scaleMatrixValues[1] = 0.0;
        scaleMatrixValues[2] = 0.0;
        scaleMatrixValues[3] = 1.0;
    }
    else
    {
        projectionMatrixValues[0] = 1.0;
        projectionMatrixValues[1] = 0.0;
        projectionMatrixValues[2] = 0.0;
        projectionMatrixValues[3] = (double) width / (double)height;

        scaleMatrixValues[0] = 1.0;
        scaleMatrixValues[1] = 0.0;
        scaleMatrixValues[2] = 0.0;
        scaleMatrixValues[3] = (double)_temp->height / (double)_temp->width;
    }

    _matrix = QMatrix2x2(projectionMatrixValues) *
              QMatrix2x2(scaleMatrixValues);

    glViewport(0, 0, width, height);
}

void GlWidget::newFrame(IplImage *img)
{
    static int prevWidth = 0, prevHeight = 0;

    _temp = img;

    if(prevWidth != _temp->width || prevHeight != _temp->height)
        resizeGL(width(), height());

    prevWidth = _temp->width;
    prevHeight = _temp->height;

    updateGL();
}

void GlWidget::makeGeometry()
{
    _vertexArray.append(QVector2D(-1.0f, -1.0f));
    _vertexArray.append(QVector2D(1.0f, -1.0f));
    _vertexArray.append(QVector2D(-1.0f, 1.0f));
    _vertexArray.append(QVector2D(1.0f, 1.0f));
}

void GlWidget::makeShaders()
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
