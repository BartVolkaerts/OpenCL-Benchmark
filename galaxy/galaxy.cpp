#include "galaxy.h"
#include <QLabel>
#include <QTime>

const int numberOfStars = 1000000;
const int numberOfGalacticCenters = 10;

Galaxy::Galaxy(Environment *environment, QWidget *parent)
    : BaseBenchmark(environment, parent)
{
    _configWidget = new QLabel("Galaxy config Widget", parent);
    _mainWidget = new GalaxyMainWidget(parent);

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(simulationStep()));
}

Galaxy::~Galaxy()
{

}

QString Galaxy::getName()
{
    return QString("Galaxy Simulation");
}

QWidget *Galaxy::getConfigWidget()
{
    return _configWidget;
}

QWidget *Galaxy::getMainWidget()
{
    return _mainWidget;
}

void Galaxy::initCL()
{
    cl_int error;


    size_t starBufferSize = numberOfStars * sizeof(cl_float4);
    size_t galacticCenterBufferSize = numberOfGalacticCenters *
        sizeof(cl_float4);

    _environment->createGLContext();
    _environment->createProgram(QStringList("galaxy/kernel.cl"));
    _kernel = _environment->getKernel("starSimulation");

    // VBO Buffer (sterren)
    _starBuffer = clCreateFromGLBuffer(_environment->getContext(),
            CL_MEM_READ_WRITE, _starVBO, &error);
    CHECK_ERR(error);
    CHECK_ERR(clFinish(_environment->getCommandQueue()));
    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(),
            1, &_starBuffer, 0, NULL, NULL));
    CHECK_ERR(clEnqueueWriteBuffer(_environment->getCommandQueue(), _starBuffer,
            CL_TRUE, 0, starBufferSize, (void *)_stars, 0, NULL, NULL));

    // Galactic Center Buffer
    _galacticCenterBuffer = clCreateBuffer(_environment->getContext(),
            CL_MEM_READ_WRITE, galacticCenterBufferSize, NULL, &error);
    CHECK_ERR(error);
    CHECK_ERR(clEnqueueWriteBuffer(_environment->getCommandQueue(),
            _galacticCenterBuffer, CL_TRUE, 0, galacticCenterBufferSize,
            (void *)_galacticCenters, 0, NULL, NULL));

    // Star speed buffer
    _starSpeedBuffer = clCreateBuffer(_environment->getContext(),
            CL_MEM_READ_WRITE, starBufferSize, NULL, &error);
    CHECK_ERR(error);
    CHECK_ERR(clEnqueueWriteBuffer(_environment->getCommandQueue(),
            _starSpeedBuffer, CL_TRUE, 0, starBufferSize, (void *)_starSpeed,
            0, NULL, NULL));

    CHECK_ERR(clSetKernelArg(_kernel, 0, sizeof(int),
            &numberOfGalacticCenters));
    CHECK_ERR(clSetKernelArg(_kernel, 1, sizeof(cl_mem),
            &_starBuffer));
    CHECK_ERR(clSetKernelArg(_kernel, 2, sizeof(cl_mem),
            &_galacticCenterBuffer));
    CHECK_ERR(clSetKernelArg(_kernel, 3, sizeof(cl_mem),
            &_starSpeedBuffer));
    CHECK_ERR(clSetKernelArg(_kernel, 4, sizeof(int),
            &numberOfStars));

    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 1,
            &_starBuffer, 0, NULL, NULL));

    clFinish(_environment->getCommandQueue());
}

void Galaxy::releaseCL()
{
    _environment->createContext();

    clReleaseKernel(_kernel);
    clReleaseMemObject(_starBuffer);
    clReleaseMemObject(_starSpeedBuffer);
    clReleaseMemObject(_galacticCenterBuffer);

    delete[] _stars;
    delete[] _galacticCenters;
    delete[] _starSpeed;
}

void Galaxy::execute()
{
    _starVBO = _mainWidget->makeGeometry(numberOfStars);
    _mainWidget->makeShaders();
    createInput(numberOfStars, numberOfGalacticCenters);
    initCL();
    _mainWidget->setActive(true);
    _timer->start(20);
}

void Galaxy::stop()
{
    _mainWidget->setActive(false);
    _timer->stop();
    releaseCL();
}

void Galaxy::simulationStep()
{
    const size_t totalNumberOfWorkItems = numberOfStars;

    CHECK_ERR(clEnqueueAcquireGLObjects(_environment->getCommandQueue(), 1,
                &_starBuffer, 0, NULL, NULL));

    CHECK_ERR(clEnqueueNDRangeKernel(_environment->getCommandQueue(),
                _kernel, 1, NULL, &totalNumberOfWorkItems, NULL, 0,
                NULL, NULL));

    CHECK_ERR(clEnqueueReleaseGLObjects(_environment->getCommandQueue(), 1,
                &_starBuffer, 0, NULL, NULL));

    clFinish(_environment->getCommandQueue());
    _mainWidget->updateGL();
}

void Galaxy::createInput(int numberOfStars, int numberOfGalacticCenters)
{
    _stars = new cl_float4[numberOfStars];
    _galacticCenters = new cl_float4[numberOfGalacticCenters];
    generateRandom(_stars, numberOfStars, 10.f);
    generateRandom(_galacticCenters, numberOfGalacticCenters, 10.f);
    _starSpeed = new cl_float4[numberOfStars];
    memset(_starSpeed, 0, sizeof(cl_float) * numberOfStars);
}

void Galaxy::generateRandom(cl_float4 *objects, int number, int boundary)
{
    qsrand(QTime::currentTime().msec());

    for (int i = 0; i < number; ++i)
    {
        objects[i].s[0] = ((float)qrand() / (float)RAND_MAX) * boundary -
            boundary / 2;
        objects[i].s[1] = ((float)qrand() / (float)RAND_MAX) * boundary -
            boundary / 2;
        objects[i].s[2] = ((float)qrand() / (float)RAND_MAX) * boundary -
            boundary / 2;
    }
}

bool Galaxy::waitForStop()
{
    return true;
}
