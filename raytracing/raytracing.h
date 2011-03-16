#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "../base_benchmark.h"

class Raytracing
    : public BaseBenchmark
{
    Q_OBJECT

    public:
        Raytracing(Environment *environment, QWidget *parent = 0);
        virtual ~Raytracing();

        QWidget *getConfigWidget();
        QWidget *getMainWidget();

        static QString getName() { return QString("Raytracing"); }

    public slots:
        void execute();

    private:
        QWidget *_configWidget;
        QWidget *_mainWidget;

};

#endif // RAYTRACING_H
