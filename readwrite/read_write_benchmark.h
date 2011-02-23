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

class Read_Write_Benchmark
        : public BaseBenchmark
{
        Q_OBJECT

        public:
            Read_Write_Benchmark(Environment *environment,
                    QWidget *parent = 0);
            virtual ~Read_Write_Benchmark();

            static QString getName();
            QWidget *getConfigWidget();
            QWidget *getMainWidget();

        public slots:
            void execute();

        protected:


        private:
            QWidget *_mainWidget;
            QWidget *_configWidget;


};

#endif // READ_WRITE_BENCHMARK_H
