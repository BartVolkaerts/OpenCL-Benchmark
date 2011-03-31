#include "io_throughput.h"

#include <QLabel>

IoThroughput::IoThroughput(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _mainWidget = new IoThroughputMainWidget(parent);
    _configWidget = new IoThroughputConfigWidget(parent);
}

IoThroughput::~IoThroughput()
{

}

QWidget *IoThroughput::getConfigWidget()
{
    return _configWidget;
}

QWidget *IoThroughput::getMainWidget()
{
    return _mainWidget;
}


void IoThroughput::initCL()
{
    _environment->createContext();
    //_environment->createProgram(QStringList("io_throughput/kernels.cl"));
    //_transferSpeedKernel = _environment->getKernel("");

}


void IoThroughput::releaseCL()
{

}

void IoThroughput::execute()
{
    const int MAX_SIZE = 1000000000;
    const int MIN_SIZE = 100;
    cl_mem devBuffer;
    cl_int error;
    QVector<double> writeSpeed;
    QVector<double> readSpeed;
    QVector<double> allocationTime;
    QVector<int> data;
    initCL();

    // Allocate memory on host:
    for (int i = _configWidget->getMinData() / 4;
            i <= _configWidget->getMaxData() / 4; i*=2)
    {
        data.append(i * sizeof(float));
        qDebug() << i << " 4 byte floats:";
        float *hostBuffer = new float[i];
        for (int j = 0; j < i; ++j)
            hostBuffer[j] = (float)j;

        // Measure allocation time:
        startTimeMeasure();
        devBuffer = clCreateBuffer(
                _environment->getContext(),
                CL_MEM_READ_WRITE,
                sizeof(float) * i,
                NULL,
                &error);
        stopTimeMeasure();
        if (error == -4)
        {
            allocationTime.append(-1);
            readSpeed.append(-1);
            writeSpeed.append(-1);
            break;
        }
        CHECK_ERR(error);
        qDebug() << "   Allocation Time:" << getTimeMeasureResults() << "s";
        allocationTime.append(getTimeMeasureResults());


        // Measure write time:
        startTimeMeasure();
        error = clEnqueueWriteBuffer(
                _environment->getCommandQueue(),
                devBuffer,
                CL_TRUE,
                0, sizeof(float) * i,
                hostBuffer,
                0, NULL, NULL);
        clFinish(_environment->getCommandQueue());
        stopTimeMeasure();
        if (error == -4)
        {
            allocationTime.append(-1);
            readSpeed.append(-1);
            writeSpeed.append(-1);
            break;
        }
        CHECK_ERR(error);
        qDebug() << "   Write Speed:" <<
            (double)(i * sizeof(float)) / getTimeMeasureResults() << "bytes/s";
        writeSpeed.append((double)(i * sizeof(float)) / getTimeMeasureResults());

        // Measure read time:
        startTimeMeasure();
        error = clEnqueueReadBuffer(
                    _environment->getCommandQueue(),
                    devBuffer,
                    CL_TRUE,
                    0, sizeof(float) * i,
                    hostBuffer,
                    0, NULL, NULL);
        clFinish(_environment->getCommandQueue());
        stopTimeMeasure();
        if (error == -4)
        {
            allocationTime.append(-1);
            readSpeed.append(-1);
            writeSpeed.append(-1);
            break;
        }
        CHECK_ERR(error);
        qDebug() << "   Read Speed:" <<
            (double)(i * sizeof(float)) / getTimeMeasureResults() << "bytes/s";
        readSpeed.append((double)(i * sizeof(float)) / getTimeMeasureResults());

        CHECK_ERR(clReleaseMemObject(devBuffer));
        delete[] hostBuffer;
    }
    _mainWidget->updateData(allocationTime, writeSpeed, readSpeed, data);
    releaseCL();

}
