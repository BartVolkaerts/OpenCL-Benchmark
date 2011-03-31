#include "io_throughput_main_widget.h"
#include <QTableWidgetItem>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>

IoThroughputMainWidget::IoThroughputMainWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    QStringList header;
    header.append("Allocation Time (s)");
    header.append("Write Speed (bytes/s)");
    header.append("Read Speed (bytes/s)");
    ui.table->setHorizontalHeaderLabels(header);
    ui.plot->setAxisTitle(2, "Speed (bytes/s)");
    ui.plot->setAxisTitle(0, "Data (bytes)");
    ui.plot->setAxisScaleEngine(2, new QwtLog10ScaleEngine());
    ui.plot->insertLegend(new QwtLegend(this));

    _readCurve = new QwtPlotCurve("Read");
    _writeCurve = new QwtPlotCurve("Write");

    _readCurve->setData(_readData);
    _writeCurve->setData(_writeData);

    _readCurve->attach(ui.plot);
    _writeCurve->attach(ui.plot);

    _readCurve->setPen(QPen(QBrush(Qt::green), 3.0));
    _writeCurve->setPen(QPen(QBrush(Qt::blue), 3.0));

}

IoThroughputMainWidget::~IoThroughputMainWidget()
{
    delete _readCurve;
    delete _writeCurve;

}
void IoThroughputMainWidget::updateData(
        QVector<double> &allocationTime,
        QVector<double> &writeSpeed,
        QVector<double> &readSpeed,
        QVector<int> &data)
{
    ui.table->setRowCount(data.count());

    QStringList headers;
    _readData.clear();
    _writeData.clear();
    for (int i = 0; i < data.count(); ++i)
    {
        headers.append(QString::number(data[i]));
        if (allocationTime[i] < 0 || writeSpeed[i] < 0 || readSpeed[i] < 0)
        {
            ui.table->setItem(i, 0,
                    new QTableWidgetItem("ERROR"));
            break;

        }
        _readData.append(QPointF((double)data[i], readSpeed[i]));
        _writeData.append(QPointF((double)data[i], writeSpeed[i]));

        ui.table->setItem(i, 0,
                new QTableWidgetItem(QString::number(allocationTime[i])));
        ui.table->setItem(i, 1,
                new QTableWidgetItem(QString::number(writeSpeed[i])));
        ui.table->setItem(i, 2,
                new QTableWidgetItem(QString::number(readSpeed[i])));
    }

    _readCurve->setData(_readData);
    _writeCurve->setData(_writeData);

    ui.table->setVerticalHeaderLabels(headers);
    ui.plot->replot();


}
