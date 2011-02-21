#ifndef FLOPS_BENCHMARK_H
#define FLOPS_BENCHMARK_H
#include "../environment.h"
#include "../base_benchmark.h"
#include "flops_main_widget.h"

#ifdef __APPLE__
#   include <OpenCL/opencl.h>
#else
#   include <CL/opencl.h>
#endif
#include <string>
#include <sys/time.h>
#include <QWidget>
#include <QMap>

class FlopsBenchmark
    : public BaseBenchmark
{
    Q_OBJECT

    public:
        FlopsBenchmark(Environment *environment, QObject *parent = 0);
        virtual ~FlopsBenchmark();

        void setData(float *input, int size);
        void readResult(float *result);

        QWidget *getConfigWidget();
        QWidget *getMainWidget();

        static QString getName();

    public slots:
        void execute();

    protected:
        void checkError(cl_int error, std::string function);
        double timeDiff(const struct timespec &end,
                const struct timespec &begin);

        double runKernel(size_t globalWorkSize);
        void showResults();
        void initCL();
        void releaseCL();

    private:
        cl_kernel _kernel;

        cl_mem _deviceInput;
        cl_mem _deviceOutput;
        int _dataSize;

        Environment *_environment;

        QWidget *_configWidget;
        FlopsMainWidget *_mainWidget;

        QMap<size_t, double> _results;

};
#endif // FLOPS_BENCHMARK_H
