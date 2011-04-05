#include "flops_benchmark.h"

#include <time.h>
#include <fstream>
#include <QLabel>
#include <iostream>
#include <cmath>

const int WORKGROUP_AMOUNT_OF_DATA = 100000;

FlopsBenchmark::FlopsBenchmark(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _environment = environment;

    _mainWidget = new FlopsMainWidget(parent);
    _configWidget = new FlopsConfigWidget(parent);
}

FlopsBenchmark::~FlopsBenchmark()
{
}

void FlopsBenchmark::initCL()
{
    _environment->createContext();
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
    _mainWidget->setWorkSizeProgress(0);
    _mainWidget->setDataProgress(0);

    initCL();

    const int OPERATION16_FLOP = 4096;

    int iterations = 100;
    int maxWorkGroupSize = _environment->getDeviceMaxWorkGroupSize();
    int workGroupData = _configWidget->getWorkSizeAmountOfData();
    int maxData = _configWidget->getDataMaxData();
    int minData = _configWidget->getDataStartData();


    float *hostData = new float[workGroupData];
    setData(hostData, workGroupData);


    _workSizeResults.clear();
    _dataResults.clear();

    double operations = (double)workGroupData *
                        (double)iterations *
                        (double)OPERATION16_FLOP;
#if 1
    for (int i = 32; i <= maxWorkGroupSize; i*=2)
    {
        _workSizeResults[i] = operations /
            runKernel(workGroupData, iterations, i);

        _workSizeResultsVector4[i] = operations /
            runVector4Kernel(workGroupData, iterations / 4, i);
        _mainWidget->setWorkSizeProgress((((int)log2(i / 32) * 100) /
                    (int)log2(maxWorkGroupSize / 32)));
    }
#endif
    for (int i = minData;i <= maxData; i*=10)
    {
        operations = (double)i *
                     (double)iterations *
                     (double)OPERATION16_FLOP;
        _dataResults[i] = operations / runKernel(i, iterations, 256);
        _dataResultsVector4[i] = operations / runVector4Kernel(i / 4, iterations, 256);
        _mainWidget->setDataProgress((((int)log(i / minData) * 100) /
                    (int)log(maxData / minData)));
    }
    _mainWidget->setDataProgress(100);
    showResults();

    releaseCL();
    delete[] hostData;
}

double FlopsBenchmark::runVector4Kernel(size_t dataSize, int iterations, size_t globalWorkSize)
{
    size_t totalWorkItems = (dataSize / globalWorkSize + 1) *
            globalWorkSize;

    cl_float4 *hostData = new cl_float4[dataSize];
    for (int i = 0; i < (int)dataSize; ++i)
    {
        hostData[i].s[0] = 1.002f + (float)i + (float)globalWorkSize;
        hostData[i].s[1] = 1.002f + (float)i - (float)globalWorkSize;
        hostData[i].s[2] = 1.002f - (float)i + (float)globalWorkSize;
        hostData[i].s[3] = 1.002f - (float)i - (float)globalWorkSize;
    }
    setData((float *)hostData, dataSize * sizeof(cl_float4));

    CHECK_ERR(clSetKernelArg(_vector4Kernel, 0, sizeof(cl_mem),
                &_deviceInput));
    CHECK_ERR(clSetKernelArg(_vector4Kernel, 1,
                sizeof(cl_float4) * globalWorkSize, NULL));
    CHECK_ERR(clSetKernelArg(_vector4Kernel, 2, sizeof(int), &dataSize));
    CHECK_ERR(clSetKernelArg(_vector4Kernel, 3, sizeof(int), &iterations));

    startTimeMeasure();

    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(), _vector4Kernel,
            1, 0, &totalWorkItems, &globalWorkSize, 0, NULL, NULL));
    CHECK_ERR(clFinish(_environment->getCommandQueue()));

    stopTimeMeasure();
    readResult(hostData, dataSize);

    CHECK_ERR(clReleaseMemObject(_deviceOutput));
    CHECK_ERR(clReleaseMemObject(_deviceInput));
    delete[] hostData;

    return getTimeMeasureResults();
}

double FlopsBenchmark::runKernel(size_t dataSize, int iterations,
        size_t globalWorkSize)
{
    size_t totalWorkItems = (dataSize / globalWorkSize + 1) *
            globalWorkSize;

    float *hostData = new float[dataSize];
    for (int i = 0; i < (int)dataSize; ++i)
        hostData[i] = 1.002f + (float)i + (float)globalWorkSize;
    setData(hostData, dataSize * sizeof(cl_float));

    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_deviceInput));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_float) * globalWorkSize,
                NULL));
    CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(int), &dataSize));
    CHECK_ERR(clSetKernelArg(_kernel, 3, sizeof(int), &iterations));

    startTimeMeasure();

    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(), _kernel,
            1, 0, &totalWorkItems, &globalWorkSize, 0, NULL, NULL));
    CHECK_ERR(clFinish(_environment->getCommandQueue()));

    stopTimeMeasure();
    readResult(hostData, dataSize);

    CHECK_ERR(clReleaseMemObject(_deviceOutput));
    CHECK_ERR(clReleaseMemObject(_deviceInput));
    delete[] hostData;

    return getTimeMeasureResults();
}

void FlopsBenchmark::showResults()
{
    _mainWidget->showResults(_workSizeResults, _dataResults,
            _workSizeResultsVector4, _dataResultsVector4);

    qDebug() << "WorkSize: \n" << _workSizeResults << "\n----------";
    qDebug() << "WorkSize (vector4): \n" << _workSizeResultsVector4 
                                      << "\n----------";
    qDebug() << "Data: \n" << _dataResults << "\n----------";
    qDebug() << "Data (vector4): \n" << _dataResultsVector4 <<
                                  "\n----------";


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
