#ifndef BASE_BENCHMARK_H
#define BASE_BENCHMARK_H
#include "environment.h"
#include <QObject>
#include <QWidget>

// TODO: Platform independent
#include <sys/time.h>
#include <time.h>

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
        struct timespec _beginTime;
        struct timespec _endTime;

};
#endif // BASE_BENCHMARK_H
