#include "mandelbrot_config_widget.h"

MandelbrotConfigWidget::MandelbrotConfigWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

}

MandelbrotConfigWidget::~MandelbrotConfigWidget()
{
}

int MandelbrotConfigWidget::getMaxIterations()
{

    return 0;
}

int MandelbrotConfigWidget::getDataType()
{

    return 0;
}

void MandelbrotConfigWidget::setResolution(int x, int y)
{

}

void MandelbrotConfigWidget::setRenderTime(double sec)
{

}
