#include "read_write_benchmark.h"

#include <QLabel>

ReadWriteBenchmark::ReadWriteBenchmark(Environment *environment,
        QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new QLabel("Hier komt mijn widget", parent);
    _configWidget = new QLabel("ConfigWidget", parent);

}

ReadWriteBenchmark::~ReadWriteBenchmark()
{

}

void ReadWriteBenchmark::execute()
{
    initCL();
    createRandomDataOnHost();
    releaseCL();
}

void ReadWriteBenchmark::createRandomDataOnHost()
{
    _data = new char[1000];
    memset((void *)_data, 0, 1000);
    std::cerr << "Clicked";
}

void ReadWriteBenchmark::initCL()
{
    _environment->createProgram(QStringList("readwrite/kernel.cl"));
    _kernel = _environment->getKernel("go");
}

void ReadWriteBenchmark::releaseCL()
{
    clReleaseKernel(_kernel);
}


QWidget *ReadWriteBenchmark::getConfigWidget()
{
    return _configWidget;
}

QWidget *ReadWriteBenchmark::getMainWidget()
{
    return _mainWidget;
}

QString ReadWriteBenchmark::getName()
{
    return QString("Read Write");
}

