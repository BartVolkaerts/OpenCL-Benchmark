#include <QLabel>

#include "raytracing.h"

Raytracing::Raytracing(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _configWidget = new QLabel("Config Widget", parent);
    _mainWidget = new QLabel("Main Widget", parent);

}

QWidget *Raytracing::getConfigWidget()
{
    return _configWidget;
}

QWidget *Raytracing::getMainWidget()
{
    return _mainWidget;
}

void Raytracing::execute()
{
}

Raytracing::~Raytracing()
{

}
