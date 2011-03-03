#ifndef BASE_BENCHMARK_H
#define BASE_BENCHMARK_H
#include "environment.h"
#include <QWidget>

// TODO: Platform independent
#ifdef _WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#   include <time.h>
#endif

class BaseBenchmark
    : public QObject
{
    Q_OBJECT

    public:
        BaseBenchmark(Environment *environment, QWidget *parent = 0);
        virtual ~BaseBenchmark();

        virtual QWidget *getConfigWidget() = 0;
        virtual QWidget *getMainWidget() = 0;
        virtual bool waitForStop();

    public slots:
        virtual void execute() = 0;
        virtual void stop();

    protected:
        void startTimeMeasure();
        void stopTimeMeasure();
        double getTimeMeasureResults();

    protected:
        Environment *_environment;

    private:
#ifdef _WIN32
        __int64 _beginTime;
        __int64 _endTime;
        double _pcFreq;
#else
        struct timespec _beginTime;
        struct timespec _endTime;
#endif

};
#endif // BASE_BENCHMARK_H
