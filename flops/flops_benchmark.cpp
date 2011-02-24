#include "flops_benchmark.h"

#include <time.h>
#include <fstream>
#include <QLabel>
#include <iostream>

const int WORKGROUP_AMOUNT_OF_DATA = 100000;

FlopsBenchmark::FlopsBenchmark(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _environment = environment;

    _mainWidget = new FlopsMainWidget(parent);
    _configWidget = new QLabel("ConfigWidget", parent);
}

FlopsBenchmark::~FlopsBenchmark()
{
}

void FlopsBenchmark::initCL()
{
    _environment->createProgram(QStringList("flops/kernel.cl"));
    _kernel = _environment->getKernel("add");
    _vector4Kernel = _environment->getKernel("addVector4");
}

void FlopsBenchmark::releaseCL()
{
    clReleaseKernel(_kernel);
    clReleaseKernel(_vector4Kernel);
}

QWidget *FlopsBenchmark::getConfigWidget()
{
    return _configWidget;
}

QWidget *FlopsBenchmark::getMainWidget()
{
    return _mainWidget;
}

void FlopsBenchmark::execute()
{
    initCL();

    float hostData[WORKGROUP_AMOUNT_OF_DATA];
    setData(hostData, WORKGROUP_AMOUNT_OF_DATA);


    _workSizeResults.clear();
    _dataResults.clear();

    for (size_t i = 1; i <= _environment->getDeviceMaxWorkGroupSize(); i*=2)
    {
        _workSizeResults[i] = WORKGROUP_AMOUNT_OF_DATA /
            runKernel(WORKGROUP_AMOUNT_OF_DATA, i);
        _workSizeResultsVector4[i] = (WORKGROUP_AMOUNT_OF_DATA) /
            runVector4Kernel(WORKGROUP_AMOUNT_OF_DATA / 4, i);
    }
    for (size_t i = 100; i < _environment->getDeviceMaxGlobalMemory(); i*=4)
    {
        _dataResults[i] = i / runKernel(i, 256);
        _dataResultsVector4[i] = i / runVector4Kernel(i / 4, 256);
    }
    showResults();

    releaseCL();
}

double FlopsBenchmark::runVector4Kernel(size_t dataSize, size_t globalWorkSize)
{
    struct timespec beginTime, endTime;
    size_t totalWorkItems = (dataSize / globalWorkSize + 1) *
            globalWorkSize;

    cl_float4 *hostData = new cl_float4[dataSize];
    setData((float *)hostData, dataSize * sizeof(cl_float4));

    CHECK_ERR(clSetKernelArg(_vector4Kernel, 0, sizeof(cl_mem),
                &_deviceInput));
    CHECK_ERR(clSetKernelArg(_vector4Kernel, 1, sizeof(cl_mem),
                &_deviceOutput));
    CHECK_ERR(clSetKernelArg(_vector4Kernel, 2,
                sizeof(cl_float4) * globalWorkSize, NULL));
    CHECK_ERR(clSetKernelArg(_vector4Kernel, 3, sizeof(int), &dataSize));

    clock_gettime(CLOCK_REALTIME, &beginTime);

    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(), _vector4Kernel,
            1, 0, &totalWorkItems, &globalWorkSize, 0, NULL, NULL));
    clFinish(_environment->getCommandQueue());

    clock_gettime(CLOCK_REALTIME, &endTime);

    clReleaseMemObject(_deviceOutput);
    clReleaseMemObject(_deviceInput);
    delete[] hostData;

    return timeDiff(endTime, beginTime);
}

double FlopsBenchmark::runKernel(size_t dataSize, size_t globalWorkSize)
{
    struct timespec beginTime, endTime;
    size_t totalWorkItems = (dataSize / globalWorkSize + 1) *
            globalWorkSize;

    float *hostData = new float[dataSize];
    setData(hostData, dataSize * sizeof(cl_float));

    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_deviceInput));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_mem), &_deviceOutput));
    CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(cl_float) * globalWorkSize,
                NULL));
    CHECK_ERR(clSetKernelArg(_kernel, 3, sizeof(int), &dataSize));

    clock_gettime(CLOCK_REALTIME, &beginTime);

    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(), _kernel,
            1, 0, &totalWorkItems, &globalWorkSize, 0, NULL, NULL));
    clFinish(_environment->getCommandQueue());

    clock_gettime(CLOCK_REALTIME, &endTime);

    clReleaseMemObject(_deviceOutput);
    clReleaseMemObject(_deviceInput);
    delete[] hostData;

    return timeDiff(endTime, beginTime);
}

void FlopsBenchmark::showResults()
{
    _mainWidget->showResults(_workSizeResults, _dataResults,
            _workSizeResultsVector4, _dataResultsVector4);
}

void FlopsBenchmark::readResult(void *result, size_t dataSize)
{
    CHECK_ERR(clEnqueueReadBuffer(_environment->getCommandQueue(), _deviceOutput,
            CL_TRUE, 0, dataSize, result, 0, NULL, NULL));
    clFinish(_environment->getCommandQueue());
}

void FlopsBenchmark::setData(void *input, size_t size)
{
    cl_int error;
    _deviceInput = clCreateBuffer(_environment->getContext(),
            CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
            size, input, &error);
    CHECK_ERR(error);

    _deviceOutput = clCreateBuffer(_environment->getContext(),
            CL_MEM_READ_WRITE,
            size, NULL, &error);
    CHECK_ERR(error);
}

double FlopsBenchmark::timeDiff(const struct timespec &end,
        const struct timespec &begin)
{
    return (double)(end.tv_sec - begin.tv_sec) +
        ((double)(end.tv_nsec - begin.tv_nsec) * 1e-9);
}

QString FlopsBenchmark::getName()
{
    return QString("Flops");
}
