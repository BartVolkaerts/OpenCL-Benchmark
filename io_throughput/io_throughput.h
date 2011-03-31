#ifndef IO_THROUGHPUT_H
#define IO_THROUGHPUT_H
#include "../base_benchmark.h"
#include "io_throughput_main_widget.h"
#include "io_throughput_config_widget.h"

class IoThroughput
    : public BaseBenchmark
{
    Q_OBJECT

    public:
        IoThroughput(Environment *environment, QWidget *parent = 0);
        virtual ~IoThroughput();

        QWidget *getConfigWidget();
        QWidget *getMainWidget();

        static QString getName() { return QString("I/O Throughput"); }

    public slots:
        void execute();


    private:
        void initCL();
        void releaseCL();

    private:
        IoThroughputMainWidget *_mainWidget;
        IoThroughputConfigWidget *_configWidget;

        void runHostDeviceBenchmark();

        cl_kernel _transferSpeedKernel();

};


#endif // IO_THROUGHPUT_H
