#include "flops_benchmark.h"

#include <time.h>
#include <fstream>
#include <QLabel>
#include <iostream>
#include <cmath>
#include <QString>

const int WORKGROUP_AMOUNT_OF_DATA = 100000;

FlopsBenchmark::FlopsBenchmark(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _environment = environment;

    _mainWidget = new FlopsMainWidget(parent);
    _configWidget = new FlopsConfigWidget(parent);

    _mainWidget->setDoubleSupport(
            _environment->deviceSupportsDouble());
    _kernel = NULL;
}

FlopsBenchmark::~FlopsBenchmark()
{
}

void FlopsBenchmark::makeKernel(QString kernel, QString type)
{
    if (_kernel)
        CHECK_ERR(clReleaseKernel(_kernel));

    _environment->createContext();
    if (type == QString("double") || type == QString("double"))
    {
        _environment->createProgram(
            QStringList(QString("flops/%1.cl").arg(kernel)),
            QString("-D TEST_TYPE=%1 -D USE_DOUBLE").arg(type));
    }
    else
    {
        _environment->createProgram(
            QStringList(QString("flops/%1.cl").arg(kernel)),
            QString("-D TEST_TYPE=%1").arg(type));
    }
    _kernel = _environment->getKernel(kernel);
}

void FlopsBenchmark::releaseCL()
{
    if (_kernel)
    {
        CHECK_ERR(clReleaseKernel(_kernel));
        _kernel = NULL;
    }
    if (_clBuffer)
    {
        CHECK_ERR(clReleaseMemObject(_clBuffer));
        _clBuffer = NULL;
    }
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
    _environment->createContext();
    _mainWidget->setWorkSizeProgress(0);
    _mainWidget->setDataProgress(0);
    _mainWidget->setCurrentDataType();

    runBenchmark(false);

    _mainWidget->setWorkSizeProgress(50);
    _mainWidget->setDataProgress(50);

    runBenchmark(true);

    _mainWidget->setWorkSizeProgress(100);
    _mainWidget->setDataProgress(100);

}

void FlopsBenchmark::runBenchmark(bool isVector)
{
    if (!isVector)
    {
        makeKernel(_configWidget->getKernelType(),
                QString(_mainWidget->getDataType()));
    }
    else if (_configWidget->getKernelType() != QString("divide"))
    {
        qDebug() << "makeVector";
        makeKernel(_configWidget->getKernelType(),
                QString(_mainWidget->getDataType() + "4"));
    }

    const int OPERATIONS_PER_THREAD = 2048;

    int iterations = 100;
    int maxWorkGroupSize = _environment->getDeviceMaxWorkGroupSize();
    int workGroupData = _configWidget->getWorkSizeAmountOfData();
    int maxData = _configWidget->getDataMaxData();
    int minData = _configWidget->getDataStartData();

    void *hostData = allocateHostMemory(workGroupData, isVector);

    double operations = (double)workGroupData *
                        (double)iterations *
                        (double)OPERATIONS_PER_THREAD;

    if (!isVector)
    {
        _singleWorkSizeResults.clear();
        _singleDataResults.clear();
    }
    else if (_configWidget->getKernelType() != QString("divide"))
    {
        _vectorWorkSizeResults.clear();
        _vectorDataResults.clear();
    }

    for (int i = 32; i <= maxWorkGroupSize; i*=2)
    {

        if (!isVector)
        {
            _singleWorkSizeResults[i] = operations /
                runKernel(workGroupData, iterations, i);
            _mainWidget->setWorkSizeProgress((((int)log2(i / 32) * 100) /
                        (int)log2(maxWorkGroupSize / 32)) / 2);
        }
        else if (_configWidget->getKernelType() != QString("divide"))
        {
            _vectorWorkSizeResults[i] = operations /
                runKernel(workGroupData, iterations / 4, i);
            _mainWidget->setWorkSizeProgress((((int)log2(i / 32) * 100) /
                        (int)log2(maxWorkGroupSize / 32)) / 2 + 50);
        }
    }
    for (int i = minData;i <= maxData; i*=10)
    {
        operations = (double)i *
                     (double)iterations *
                     (double)OPERATIONS_PER_THREAD;

        if (!isVector)
        {
            _singleDataResults[i] = operations / runKernel(i, iterations, 256);
            _mainWidget->setDataProgress((((int)log(i / minData) * 100) /
                        (int)log(maxData / minData)) / 2);
        }
        else if (_configWidget->getKernelType() != QString("divide"))
        {
            _vectorDataResults[i] = operations / runKernel(i, iterations / 4, 256);
            _mainWidget->setDataProgress((((int)log(i / minData) * 100) /
                        (int)log(maxData / minData)) / 2);
        }
    }
    showResults();

    releaseCL();
    releaseHostMemory(hostData);
}

double FlopsBenchmark::runKernel(size_t dataSize, int iterations,
        size_t globalWorkSize)
{
    size_t totalWorkItems = (dataSize / globalWorkSize + 1) *
            globalWorkSize;

    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_clBuffer));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(int), &dataSize));
    CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(int), &iterations));

    startTimeMeasure();

    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(), _kernel,
            1, 0, &totalWorkItems, &globalWorkSize, 0, NULL, NULL));
    CHECK_ERR(clFinish(_environment->getCommandQueue()));

    stopTimeMeasure();

    return getTimeMeasureResults();
}

void *FlopsBenchmark::allocateHostMemory(int size, bool isVector)
{
    void *hostData;

    if (!isVector)
    {
        switch (_mainWidget->getDataTypeId())
        {
            case FlopsMainWidget::FLOAT:
            default:
                hostData = new cl_float[size];
                createCLBuffer(hostData, _bufferSize = sizeof(cl_float) * size);
                break;
            case FlopsMainWidget::DOUBLE:
                hostData = new cl_double[size];
                createCLBuffer(hostData, _bufferSize = sizeof(cl_double) * size);
                break;
            case FlopsMainWidget::INTEGER:
                hostData = new cl_int[size];
                createCLBuffer(hostData, _bufferSize = sizeof(cl_int) * size);
                break;
            case FlopsMainWidget::HALF:
                hostData = new cl_half[size];
                createCLBuffer(hostData, _bufferSize = sizeof(cl_half) * size);
                break;
        }
        return hostData;
    }
    else
    {
        switch (_mainWidget->getDataTypeId())
        {
            case FlopsMainWidget::FLOAT:
            default:
                hostData = new cl_float4[size / 4];
                createCLBuffer(hostData, _bufferSize = sizeof(cl_float4) * size / 4);
                break;
            case FlopsMainWidget::DOUBLE:
                hostData = new cl_double4[size / 4];
                createCLBuffer(hostData, _bufferSize = sizeof(cl_double4) * size / 4);
                break;
            case FlopsMainWidget::INTEGER:
                hostData = new cl_int4[size / 4];
                createCLBuffer(hostData, _bufferSize = sizeof(cl_int4) * size / 4);
                break;
#if 0
            case FlopsMainWidget::HALF:
                hostData = new cl_half4[size / 4];
                createCLBuffer(hostData, _bufferSize = sizeof(cl_half4) * size * 4);
                break;
#endif
        }
        return hostData;
    }
}

void FlopsBenchmark::releaseHostMemory(void *hostData)
{
    switch (_mainWidget->getDataTypeId())
    {
        case FlopsMainWidget::FLOAT:
            delete[] (cl_float *)hostData;
            return;
        case FlopsMainWidget::DOUBLE:
            delete[] (cl_double *)hostData;
            return;
        case FlopsMainWidget::INTEGER:
            delete[] (cl_int *)hostData;
            return;
        case FlopsMainWidget::HALF:
            delete[] (cl_half *)hostData;
            return;
    }
}

void FlopsBenchmark::createCLBuffer(void *input, size_t size)
{
    cl_int error;
    _clBuffer = clCreateBuffer(_environment->getContext(),
            CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
            size, input, &error);
    CHECK_ERR(error);
}

void FlopsBenchmark::showResults()
{
    _mainWidget->showResults(_singleWorkSizeResults, _singleDataResults,
            _vectorWorkSizeResults, _vectorDataResults);
}

void FlopsBenchmark::readResult(void *result, size_t dataSize)
{
    CHECK_ERR(clEnqueueReadBuffer(_environment->getCommandQueue(), _clBuffer,
            CL_TRUE, 0, dataSize, result, 0, NULL, NULL));
    clFinish(_environment->getCommandQueue());
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
