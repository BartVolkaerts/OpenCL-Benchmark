#include "base_benchmark.h"
#include <QLabel>

BaseBenchmark::BaseBenchmark(Environment *environment, QWidget *parent)
    : QObject(parent)
{
    _environment = environment;
    _beginTime.tv_sec = 0;
    _beginTime.tv_nsec = 0;
    _endTime.tv_sec = 0;
    _endTime.tv_nsec = 0;
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

void BaseBenchmark::startTimeMeasure()
{
    clock_gettime(CLOCK_REALTIME, &_beginTime);
}

void BaseBenchmark::stopTimeMeasure()
{
    clock_gettime(CLOCK_REALTIME, &_endTime);
}

double BaseBenchmark::getTimeMeasureResults()
{
    return (double)(_endTime.tv_sec - _beginTime.tv_sec) +
        ((double)(_endTime.tv_nsec - _beginTime.tv_nsec) * 1e-9);
}
