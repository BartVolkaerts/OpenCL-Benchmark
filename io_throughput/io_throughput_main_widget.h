#ifndef IO_THROUGHPUT_MAIN_WIDGET_H
#define IO_THROUGHPUT_MAIN_WIDGET_H
#include <QWidget>
#include <qwt_plot_curve.h>
#include "ui_io_throughput_main_widget.h"

class IoThroughputMainWidget
    : public QWidget
{
    Q_OBJECT

    public:
        IoThroughputMainWidget(QWidget *parent = 0);
        ~IoThroughputMainWidget();

        void updateData(
                QVector<double> &allocationTime,
                QVector<double> &writeSpeed,
                QVector<double> &readSpeed,
                QVector<int> &data);

    private:
        Ui::IoThroughputMainWidget ui;
        QwtPlotCurve *_readCurve;
        QwtPlotCurve *_writeCurve;

        QPolygonF _readData;
        QPolygonF _writeData;
};

#endif /// IO_THROUGHPUT_MAIN_WIDGET_H
