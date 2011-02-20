#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include "ui_main_window.h"
#include "flops/flops_benchmark.h"
#include "environment.h"
#include "base_benchmark.h"
#include "flops/flops_benchmark.h"

#include <QMainWindow>

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

    protected slots:
        void platformBoxChanged(const QString &currentIndex);
        void deviceBoxChanged(const QString &currentIndex);


    private:
        Ui::MainWindow ui;
        Environment *_environment;
        BaseBenchmark *_benchmark;

        QMap<QString, cl_platform_id> _platforms;
        QMap<QString, cl_device_id> _devices;

};
#endif // MAINWINDOW
