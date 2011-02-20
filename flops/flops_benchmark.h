#ifndef FLOPS_BENCHMARK_H
#define FLOPS_BENCHMARK_H
#include "../environment.h"
#include "../base_benchmark.h"

#ifdef __APPLE__
#   include <OpenCL/opencl.h>
#else
#   include <CL/opencl.h>
#endif
#include <string>
#include <sys/time.h>
#include <QWidget>

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

    public slots:
        void execute();

    protected:
        void checkError(cl_int error, std::string function);
        size_t readFile(std::string filename, char **source);
        void printBuildLog();
        double timeDiff(const struct timespec &end,
                const struct timespec &begin);

    private:
        cl_kernel _kernel;

        cl_mem _deviceInput;
        cl_mem _deviceOutput;
        int _dataSize;

        Environment *_environment;

        QWidget *_configWidget;
        QWidget *_mainWidget;

};
#endif // FLOPS_BENCHMARK_H
