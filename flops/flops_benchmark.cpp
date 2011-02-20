#include "flops_benchmark.h"

#include <time.h>
#include <fstream>
#include <QLabel>
#include <iostream>

FlopsBenchmark::FlopsBenchmark(Environment *environment, QObject *parent)
    : BaseBenchmark(environment, parent)
{
    _environment = environment;

    _environment->createProgram(QStringList("flops/kernel.cl"));
    _kernel = _environment->getKernel("add");

}

FlopsBenchmark::~FlopsBenchmark()
{
    clReleaseMemObject(_deviceInput);
    clReleaseMemObject(_deviceOutput);
    clReleaseKernel(_kernel);
}

QWidget *FlopsBenchmark::getConfigWidget()
{
    return _configWidget = new QLabel("flopsconfig");
}

QWidget *FlopsBenchmark::getMainWidget()
{
    return _mainWidget = new QLabel("Mainwidget");
}

void FlopsBenchmark::execute()
{
    cl_int error;
    float hostData[1000000];
    setData(hostData, 1000000);

    struct timespec beginTime, endTime;
    size_t globalWorkSize = 512;
    error = clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_deviceInput);
    checkError(error, "clSetKernelArg()");

    error = clSetKernelArg(_kernel, 1, sizeof(cl_mem), &_deviceOutput);
    checkError(error, "clSetKernelArg()");

    error = clSetKernelArg(_kernel, 2, sizeof(int), &_dataSize);
    checkError(error, "clSetKernelArg()");

    size_t totalWorkItems = ((size_t)_dataSize / globalWorkSize + 1) *
            globalWorkSize;

    clock_gettime(CLOCK_REALTIME, &beginTime);
    error = clEnqueueNDRangeKernel(_environment->getCommandQueue(), _kernel,
            1, 0, &totalWorkItems, &globalWorkSize, 0, NULL, NULL);
    checkError(error, "clEnqueueNDRangeKernel()");
    clFinish(_environment->getCommandQueue());
    clock_gettime(CLOCK_REALTIME, &endTime);
    ((QLabel *)_mainWidget)->setText(QString("%1")
        .arg(timeDiff(endTime, beginTime)));
}

void FlopsBenchmark::readResult(float *result)
{
    cl_int error;
    error = clEnqueueReadBuffer(_environment->getCommandQueue(), _deviceOutput,
            CL_TRUE, 0, sizeof(float) * _dataSize, result, 0, NULL, NULL);
    checkError(error, "clEnqueueReadBuffer()");
    clFinish(_environment->getCommandQueue());
}

void FlopsBenchmark::setData(float *input, int size)
{
    _dataSize = size;
    cl_int error;
    _deviceInput = clCreateBuffer(_environment->getContext(),
            CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
            sizeof(float) * size, input, &error);
    checkError(error, "clCreateBuffer() 1");

    _deviceOutput = clCreateBuffer(_environment->getContext(),
            CL_MEM_READ_WRITE,
            sizeof(float) * size, NULL, &error);
    checkError(error, "clCreateBuffer() 2");
}

void FlopsBenchmark::checkError(cl_int error, std::string function)
{
    if (error != CL_SUCCESS)
    {
        std::cerr << "ERROR: Code: " << error << std::endl 
            << "    Function: " << function << std::endl;
        exit(1);
    }
}

double FlopsBenchmark::timeDiff(const struct timespec &end,
        const struct timespec &begin)
{
    return (double)(end.tv_sec - begin.tv_sec) +
        ((double)(end.tv_nsec - begin.tv_nsec) * 1e-9);
}
