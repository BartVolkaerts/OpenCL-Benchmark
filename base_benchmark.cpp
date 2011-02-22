#include "base_benchmark.h"
#include <QLabel>

BaseBenchmark::BaseBenchmark(Environment *environment, QWidget *parent)
    : QObject(parent)
{
    _environment = environment;
}

BaseBenchmark::~BaseBenchmark()
{
}

QWidget *BaseBenchmark::getConfigWidget()
{
    return new QLabel("Configuration");
}

QWidget *BaseBenchmark::getMainWidget()
{
    return new QLabel("MainWidget");
}
