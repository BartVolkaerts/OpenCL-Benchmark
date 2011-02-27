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

void BaseBenchmark::stop()
{
}

bool BaseBenchmark::waitForStop()
{
    return false;
}
