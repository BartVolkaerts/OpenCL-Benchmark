#ifndef IO_THROUGHPUT_CONFIG_WIDGET_H
#define IO_THROUGHPUT_CONFIG_WIDGET_H
#include <QWidget>
#include "ui_io_throughput_config_widget.h"

class IoThroughputConfigWidget
    : public QWidget
{
    Q_OBJECT

    public:
        IoThroughputConfigWidget(QWidget *parent = 0);
        ~IoThroughputConfigWidget();

        int getMaxData();
        int getMinData();

    private slots:
        void setMinData(int min);
        void setMaxData(int max);

    private:
        Ui::IoThroughputConfigWidget ui;

};
#endif // IO_THROUGHPUT_CONFIG_WIDGET_H
