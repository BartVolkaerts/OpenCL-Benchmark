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
#ifdef _WIN32
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);

    _pcFreq = (double)li.QuadPart;

    QueryPerformanceCounter(&li);
    _beginTime = li.QuadPart;

#else
    clock_gettime(CLOCK_REALTIME, &_beginTime);
#endif
}

void BaseBenchmark::stopTimeMeasure()
{
#ifdef _WIN32
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);

    _pcFreq = (double)li.QuadPart;

    QueryPerformanceCounter(&li);
    _endTime = li.QuadPart;
#else
    clock_gettime(CLOCK_REALTIME, &_endTime);
#endif
}

double BaseBenchmark::getTimeMeasureResults()
{
#ifdef _WIN32
    return (double)(_endTime - _beginTime)/_pcFreq;
#else
    return (double)(_endTime.tv_sec - _beginTime.tv_sec) +
        ((double)(_endTime.tv_nsec - _beginTime.tv_nsec) * 1e-9);
#endif
}
