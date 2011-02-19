#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QMap>
#include <QtGlobal>
#include <QDebug>
#include <QFile>

#define CHECK_ERR(code) \
    if ((code) != CL_SUCCESS)\
    {\
        qCritical("ERROR:\n\t"\
                "Code: %d\n\t"\
                "Filename: %s\n\t"\
                "Line: %d\n", (code), __FILE__, __LINE__);\
                exit(code);\
    }

#ifdef __APPLE__
#   include <OpenCL/cl.h>
#else
#   include <CL/cl.h>
#endif


class Environment
    : public QObject
{
    Q_OBJECT

    public:
        Environment(QObject *parent = 0);
        virtual ~Environment();

        // Program:
        void createProgram(QStringList filenames);
        cl_kernel getKernel(QString name);

        // Command Queue methods:
        void createCommandQueue();

        // Context methods:
        void createContext();
        void createGLContext();

        // Device methods:
        cl_device_id getCurrentDevice();
        QMap<QString, cl_device_id> getDevicesMap();
        static cl_uint getDevicesNumber(cl_platform_id platform,
                cl_device_type type = CL_DEVICE_TYPE_ALL);
        QString getDeviceName(cl_device_id device);
        void setDevice(cl_device_id device);

        // Platform methods:
        cl_platform_id getCurrentPlatform();
        QMap<QString, cl_platform_id> getPlatformsMap();
        static cl_uint getPlatformsNumber();
        QString getPlatformInfo(cl_platform_id platform,
                cl_platform_info info);
        void setPlatform(cl_platform_id platform);

    protected:
        size_t readFile(QString filename, char **source);
        void printBuildLog();

    private:
        cl_device_id _currentDevice;
        cl_platform_id _currentPlatform;
        cl_context _context;
        cl_command_queue _commandQueue;
        cl_program _program;

};

#endif // ENVIRONMENT_H
