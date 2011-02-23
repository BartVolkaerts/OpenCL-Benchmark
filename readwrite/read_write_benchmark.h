#ifndef READ_WRITE_BENCHMARK_H
#define READ_WRITE_BENCHMARK_H
#include "../environment.h"
#include "../base_benchmark.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include <time.h>

class ReadWriteBenchmark
        : public BaseBenchmark
{
        Q_OBJECT

        public:
            ReadWriteBenchmark(Environment *environment,
                    QWidget *parent = 0);
            virtual ~ReadWriteBenchmark();

            static QString getName();
            QWidget *getConfigWidget();
            QWidget *getMainWidget();

        public slots:
            void execute();

        protected:
            void initCL();
            void releaseCL();
            void createRandomDataOnHost();

        private:
            cl_kernel _kernel;

            QWidget *_mainWidget;
            QWidget *_configWidget;

            char *_data;


};

#endif // READ_WRITE_BENCHMARK_H
