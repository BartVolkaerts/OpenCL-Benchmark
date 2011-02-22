#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include "ui_main_window.h"
#include "environment.h"
#include "base_benchmark.h"
#include "flops/flops_benchmark.h"
#include "readwrite/read_write_benchmark.h"

#include <QMainWindow>
#include <QVector>

class MainWindow
    : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);
        virtual ~MainWindow();

    protected:
        void setPlatformBox();
        void setDevicesBox();
        void addBenchmark(const QString &name, BaseBenchmark *benchmark);

    protected slots:
        void platformBoxChanged(const QString &currentIndex);
        void deviceBoxChanged(const QString &currentIndex);
        void launchBenchmark();
        void setBenchmarkWidgets(const QString &benchmark);

    private:
        BaseBenchmark *getSelectedBenchmark();


    private:
        Ui::MainWindow ui;
        Environment *_environment;
        QMap<QString, BaseBenchmark *> _benchmarks;

        QMap<QString, cl_platform_id> _platforms;
        QMap<QString, cl_device_id> _devices;

        BaseBenchmark *_currentBenchmark;

};
#endif // MAINWINDOW
