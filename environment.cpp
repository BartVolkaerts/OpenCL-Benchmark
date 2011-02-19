#include "environment.h"

Environment::Environment(QObject *parent)
    : QObject(parent)
{
    _context = NULL;
    _commandQueue = NULL;
}

Environment::~Environment()
{
    CHECK_ERR(clReleaseCommandQueue(_commandQueue));
    CHECK_ERR(clReleaseContext(_context));
}

// Program:
// ---------------------------------------------------------------------------

cl_kernel Environment::getKernel(QString name)
{
    cl_kernel kernel;
    cl_int error;
    kernel = clCreateKernel(_program, name.toAscii().constData(), &error);
    CHECK_ERR(error);
    return kernel;
}

void Environment::createProgram(QStringList filenames)
{
    char **sources = new char *[filenames.count()];
    size_t *sizes = new size_t[filenames.count()];
    cl_int error;

    for (int i = 0; i < filenames.size(); ++i)
    {
        sizes[i] = readFile(filenames[i], &sources[i]);
    }

    _program = clCreateProgramWithSource(_context, filenames.size(),
            (const char **)sources, sizes, &error);
    CHECK_ERR(error);

    error = clBuildProgram(_program, 1, &_currentDevice, NULL, NULL, NULL);
    if (error != CL_SUCCESS)
        printBuildLog();
    CHECK_ERR(error);

    for (int i = 0; i < filenames.size(); ++i)
    {
        delete[] sources[i];
    }
    delete[] sizes;
    delete[] sources;
}

size_t Environment::readFile(QString filename, char **source)
{
    QFile file(filename);
    size_t size;
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    size = file.size();

    *source = new char[size + 1];
    qstrcpy(*source, file.readAll().constData());
    file.close();

    return size;
}

void Environment::printBuildLog()
{
    char *log;
    size_t size;

    clGetProgramBuildInfo(_program, _currentDevice, CL_PROGRAM_BUILD_LOG, 0, NULL,
            &size);
    log = new char[size + 1];
    clGetProgramBuildInfo(_program, _currentDevice, CL_PROGRAM_BUILD_LOG, size, log,
            NULL);
    log[size] = '\0';
    qFatal("Build log:\n%s\n", log);
    free(log);
}


// Command Queue:
// ---------------------------------------------------------------------------

void Environment::createCommandQueue()
{
    cl_int error;
    _commandQueue = clCreateCommandQueue(_context, _currentDevice, 0, &error);
    CHECK_ERR(error);
}

// Context:
// ---------------------------------------------------------------------------

void Environment::createContext()
{
    cl_int error;
    if (_context)
        CHECK_ERR(clReleaseContext(_context));
    _context = clCreateContext(NULL, 1, &_currentDevice, NULL, NULL, &error);
    CHECK_ERR(error);
}

void Environment::createGLContext()
{
    // TODO: Implement function
}

// Devices:
// ---------------------------------------------------------------------------

cl_device_id Environment::getCurrentDevice()
{
    return _currentDevice;
}

void Environment::setDevice(cl_device_id device)
{
    _currentDevice = device;
}

QMap<QString, cl_device_id> Environment::getDevicesMap()
{
    QMap<QString, cl_device_id> result;
    cl_device_id *devices =
        new cl_device_id[getDevicesNumber(_currentPlatform)];

    CHECK_ERR(clGetDeviceIDs(_currentPlatform, CL_DEVICE_TYPE_ALL,
                getDevicesNumber(_currentPlatform), devices, NULL));

    for (unsigned int i = 0; i < getDevicesNumber(_currentPlatform); ++i)
    {
        result.insert(getDeviceName(devices[i]), devices[i]);
    }
    delete[] devices;
    return result;

}

cl_uint Environment::getDevicesNumber(cl_platform_id platform,
        cl_device_type type)
{
    cl_uint number;
    CHECK_ERR(clGetDeviceIDs(platform, type, 0, NULL, &number));
    return number;
}

QString Environment::getDeviceName(cl_device_id device)
{
    size_t stringSize;
    char *parameter;
    CHECK_ERR(clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &stringSize));
    parameter = new char[stringSize];
    CHECK_ERR(clGetDeviceInfo(device, CL_DEVICE_NAME, stringSize,
                (void *)parameter, NULL));
    QString result(parameter);
    delete[] parameter;
    return result;
}

// Platforms:
// ---------------------------------------------------------------------------

cl_platform_id Environment::getCurrentPlatform()
{
    return _currentPlatform;
}

void Environment::setPlatform(cl_platform_id platform)
{
    _currentPlatform = platform;
}

QMap<QString, cl_platform_id> Environment::getPlatformsMap()
{
    cl_platform_id *platforms = new cl_platform_id[getPlatformsNumber()];
    CHECK_ERR(clGetPlatformIDs(getPlatformsNumber(), platforms, NULL));

    QMap<QString, cl_platform_id> result;
    for (unsigned int i = 0; i < getPlatformsNumber(); ++i)
    {
        result.insert(getPlatformInfo(platforms[i],
                    CL_PLATFORM_NAME), platforms[i]);

    }
    delete[] platforms;
    return result;
}

QString Environment::getPlatformInfo(cl_platform_id platform,
        cl_platform_info info)
{
    size_t stringSize;
    char *parameter;
    CHECK_ERR(clGetPlatformInfo(platform, info, 0, NULL, &stringSize));
    parameter = new char[stringSize];
    CHECK_ERR(clGetPlatformInfo(platform, info, stringSize,
                (void *)parameter, NULL));
    QString result(parameter);
    delete[] parameter;
    return result;
}

cl_uint Environment::getPlatformsNumber()
{
    cl_uint number;
    CHECK_ERR(clGetPlatformIDs(0, NULL, &number));
    return number;
}
