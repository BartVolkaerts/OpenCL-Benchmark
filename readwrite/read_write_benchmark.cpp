#include "read_write_benchmark.h"

#include <QLabel>

Read_Write_Benchmark::Read_Write_Benchmark(Environment *environment,
        QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new QLabel("Main Widget", parent);
    _configWidget = new QLabel("Config Widget", parent);

}

Read_Write_Benchmark::~Read_Write_Benchmark()
{

}

void Read_Write_Benchmark::execute()
{

}


QWidget *Read_Write_Benchmark::getConfigWidget()
{
    return _configWidget;
}

QWidget *Read_Write_Benchmark::getMainWidget()
{
    return _mainWidget;
}

QString Read_Write_Benchmark::getName()
{
    return QString("Read Write");
}

