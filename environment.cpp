#include "environment.h"


// TODO: Include headers for other platforms.
#include <CL/cl_gl_ext.h>
#include <CL/cl_gl.h>
#include <GL/glx.h>
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"

Environment::Environment(QObject *parent)
    : QObject(parent)
{
    _program = NULL;
    _context = NULL;
    _commandQueue = NULL;
}

Environment::~Environment()
{
    if (_program)
        CHECK_ERR(clReleaseProgram(_program));
    if (_commandQueue)
        CHECK_ERR(clReleaseCommandQueue(_commandQueue));
    if(_context)
        CHECK_ERR(clReleaseContext(_context));
}

// Program:
// ----------------------------------------------------------------------------

cl_kernel Environment::getKernel(QString name)
{
    cl_kernel kernel;
    cl_int error;
    kernel = clCreateKernel(_program, name.toAscii().constData(), &error);
    CHECK_ERR(error);
    return kernel;
}

void Environment::createProgram(QStringList filenames, QString options)
{
    char **sources = new char *[filenames.count()];
    size_t *sizes = new size_t[filenames.count()];
    const char *optionsArray = NULL;
    cl_int error;

    for (int i = 0; i < filenames.size(); ++i)
    {
        sizes[i] = readFile(filenames[i], &sources[i]);
    }
    if (_program)
        CHECK_ERR(clReleaseProgram(_program));

    _program = clCreateProgramWithSource(_context, filenames.size(),
            (const char **)sources, sizes, &error);
    CHECK_ERR(error);

    if (!options.isNull())
        optionsArray = options.toAscii().constData();

    error = clBuildProgram(_program, 1, &_currentDevice, optionsArray, NULL, NULL);
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

    clGetProgramBuildInfo(_program, _currentDevice, CL_PROGRAM_BUILD_LOG, 0,
            NULL, &size);
    log = new char[size + 1];
    clGetProgramBuildInfo(_program, _currentDevice, CL_PROGRAM_BUILD_LOG,
            size, log, NULL);
    log[size] = '\0';
    qWarning("Build log:\n%s\n", log);
    free(log);
}


// Command Queue:
// ----------------------------------------------------------------------------

cl_command_queue Environment::getCommandQueue()
{
    return _commandQueue;
}

void Environment::createCommandQueue()
{
    cl_int error;
    if (_commandQueue)
        CHECK_ERR(clReleaseCommandQueue(_commandQueue));
    _commandQueue = clCreateCommandQueue(_context, _currentDevice, 0, &error);
    CHECK_ERR(error);
}

// Context:
// ----------------------------------------------------------------------------

void Environment::createContext()
{
    cl_int error;
    if (_context)
        CHECK_ERR(clReleaseContext(_context));
    _context = clCreateContext(NULL, 1, &_currentDevice, NULL, NULL, &error);
    CHECK_ERR(error);
    createCommandQueue();
}

void Environment::createGLContext()
{
    // TODO: Works only on linux, impliment on multiple platforms.
    cl_int error;

    if (_context)
        CHECK_ERR(clReleaseContext(_context));
    cl_context_properties props[] =
    {
        CL_GL_CONTEXT_KHR,  (cl_context_properties)glXGetCurrentContext(),
        CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)_currentPlatform,
        0
    };
    _context = clCreateContext(props, 1, &_currentDevice, NULL, NULL, &error);
    CHECK_ERR(error);
    createCommandQueue();
}

cl_context Environment::getContext()
{
    return _context;
}

int Environment::getContextReferenceCount()
{
    int count;
    CHECK_ERR(clGetContextInfo(_context, CL_CONTEXT_REFERENCE_COUNT, sizeof(int),
            &count, NULL));
    return count;
}

// Devices:
// ----------------------------------------------------------------------------

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
        qDebug() << getDeviceName(devices[i]);
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

QString Environment::getDeviceExtensions()
{
    size_t stringSize;
    char *parameter;
    CHECK_ERR(clGetDeviceInfo(_currentDevice, CL_DEVICE_EXTENSIONS, 0, NULL, &stringSize));
    parameter = new char[stringSize];
    CHECK_ERR(clGetDeviceInfo(_currentDevice, CL_DEVICE_EXTENSIONS, stringSize,
                (void *)parameter, NULL));
    QString result(parameter);
    delete[] parameter;
    return result;
}

bool Environment::deviceSupportsDouble()
{
    QString extensions = getDeviceExtensions();
    return extensions.contains("cl_khr_fp64");
}

size_t Environment::getDeviceMaxWorkGroupSize()
{
    size_t result;
    CHECK_ERR(clGetDeviceInfo(_currentDevice, CL_DEVICE_MAX_WORK_GROUP_SIZE,
                sizeof(size_t), &result, NULL));
    return result;
}

size_t Environment::getDeviceMaxGlobalMemory()
{
    size_t result;
    CHECK_ERR(clGetDeviceInfo(_currentDevice, CL_DEVICE_GLOBAL_MEM_SIZE,
                sizeof(size_t), &result, NULL));
    return result;
}

// Platforms:
// ----------------------------------------------------------------------------

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
        qDebug() << getPlatformInfo(platforms[i],
                    CL_PLATFORM_NAME);

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
