#include "read_write_benchmark.h"

Read_Write_Benchmark::Read_Write_Benchmark(Environment *environment, QObject *parent)
    : BaseBenchmark(environment, parent)
{
    _environment = environment;

}

Read_Write_Benchmark::~Read_Write_Benchmark()
{

}

void Read_Write_Benchmark::execute()
{

}

QString Read_Write_Benchmark::getName()
{
    return QString("Read Write");
}

