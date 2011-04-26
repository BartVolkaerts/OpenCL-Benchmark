#ifndef FLOPS_BENCHMARK_H
#define FLOPS_BENCHMARK_H
#include "../environment.h"
#include "../base_benchmark.h"
#include "flops_main_widget.h"
#include "flops_config_widget.h"

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
        FlopsBenchmark(Environment *environment, QWidget *parent = 0);
        virtual ~FlopsBenchmark();

        void createCLBuffer(void *input, size_t size);
        void readResult(void *result, size_t dataSize);

        QWidget *getConfigWidget();
        QWidget *getMainWidget();

        static QString getName();

    public slots:
        void execute();

    protected:
        void checkError(cl_int error, std::string function);
        double timeDiff(const struct timespec &end,
                const struct timespec &begin);

        double runKernel(size_t dataSize, int iterations, size_t globalWorkSize);
        double runVector4Kernel(size_t dataSize, int iterations, size_t globalWorkSize);
        void showResults();
        void makeKernel(QString kernel, QString type);
        void releaseCL();
        void *allocateHostMemory(int size, bool isVector);
        void releaseHostMemory(void *hostBuffer);
        void runBenchmark(bool isVector);

    private:
        cl_kernel _kernel;

        cl_mem _clBuffer;
        size_t _bufferSize;

        Environment *_environment;

        FlopsConfigWidget *_configWidget;
        FlopsMainWidget *_mainWidget;

        QMap<size_t, double> _singleWorkSizeResults;
        QMap<size_t, double> _singleDataResults;
        QMap<size_t, double> _vectorWorkSizeResults;
        QMap<size_t, double> _vectorDataResults;

};
#endif // FLOPS_BENCHMARK_H
