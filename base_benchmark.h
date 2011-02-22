#ifndef BASE_BENCHMARK_H
#define BASE_BENCHMARK_H
#include "environment.h"
#include <QObject>
#include <QWidget>

class BaseBenchmark
    : public QObject
{
    Q_OBJECT

    public:
        BaseBenchmark(Environment *environment, QWidget *parent = 0);
        virtual ~BaseBenchmark();

        virtual QWidget *getConfigWidget();
        virtual QWidget *getMainWidget();

    public slots:
        virtual void execute() = 0;

    protected:
        Environment *_environment;

};
#endif // BASE_BENCHMARK_H
