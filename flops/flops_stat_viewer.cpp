#include "flops_stat_viewer.h"

#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <QPen>
#include <QBrush>

FlopsStatViewer::FlopsStatViewer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    // Setup graphs
    ui.workSizePlot->setAxisTitle(0, "Operations");
    ui.workSizePlot->setAxisTitle(2, "Workgroup size");
    ui.workSizePlot->setAxisScaleEngine(2, new QwtLog10ScaleEngine());
    ui.workSizePlot->setAxisScaleEngine(0, new QwtLog10ScaleEngine());
    ui.workSizePlot->insertLegend(new QwtLegend(this));

    ui.dataPlot->setAxisTitle(0, "Operations");
    ui.dataPlot->setAxisTitle(2, "Data");
    ui.dataPlot->setAxisScaleEngine(2, new QwtLog10ScaleEngine());
    ui.dataPlot->setAxisScaleEngine(0, new QwtLog10ScaleEngine());
    ui.dataPlot->insertLegend(new QwtLegend(this));

    _singleWorkSizeCurve = new QwtPlotCurve("Single");
    _singleWorkSizeCurve->setData(_singleWorkSizeData);
    _singleWorkSizeCurve->attach(ui.workSizePlot);

    _vectorWorkSizeCurve = new QwtPlotCurve("4D Vector");
    _vectorWorkSizeCurve->setData(_vectorWorkSizeData);
    _vectorWorkSizeCurve->attach(ui.workSizePlot);

    _singleDataCurve = new QwtPlotCurve("Single");
    _singleDataCurve->setData(_singleDataData);
    _singleDataCurve->attach(ui.dataPlot);

    _vectorDataCurve = new QwtPlotCurve("4D Vector");
    _vectorDataCurve->setData(_vectorWorkSizeData);
    _vectorDataCurve->attach(ui.dataPlot);

    _singleWorkSizeCurve->setPen(QPen(QBrush(Qt::red), 3.0));
    _vectorWorkSizeCurve->setPen(QPen(QBrush(Qt::blue), 3.0));

    _singleDataCurve->setPen(QPen(QBrush(Qt::red), 3.0));
    _vectorDataCurve->setPen(QPen(QBrush(Qt::blue), 3.0));

    // Setup header in table
    QStringList header;
    header.append("Sigle");
    header.append("4D Vector");

    ui.workSizeTable->setHorizontalHeaderLabels(header);
    ui.dataTable->setHorizontalHeaderLabels(header);
}

FlopsStatViewer::~FlopsStatViewer()
{
    delete _singleWorkSizeCurve;
    delete _vectorWorkSizeCurve;
    delete _singleDataCurve;
    delete _vectorDataCurve;
}

void FlopsStatViewer::setWorkSizeProgress(int progress)
{
    ui.dataProgressBar->setValue(progress);
}

void FlopsStatViewer::setDataProgress(int progress)
{
    ui.workSizeProgressBar->setValue(progress);
}

void FlopsStatViewer::showResults(
        QMap<size_t, double> &singleWorkSizeData,
        QMap<size_t, double> &singleDataData,
        QMap<size_t, double> &vectorWorkSizeData,
        QMap<size_t, double> &vectorDataData)
{
    QMap<size_t, double>::const_iterator it;
    QStringList header;

    ui.workSizeTable->setRowCount(singleWorkSizeData.count());
    ui.dataTable->setRowCount(singleDataData.count());

    _singleWorkSizeData.clear();
    int i = 0;
    for (it = singleWorkSizeData.constBegin();
            it != singleWorkSizeData.constEnd(); ++it)
    {
        header.append(QString::number(it.key()));
        ui.workSizeTable->setItem(i, 0,
                new QTableWidgetItem(QString::number(it.value())));
        _singleWorkSizeData.append(QPointF(it.key(), it.value()));
        ++i;
    }
    _singleWorkSizeCurve->setData(_singleWorkSizeData);
    ui.workSizeTable->setVerticalHeaderLabels(header);

    _vectorWorkSizeData.clear();
    i = 0;
    for (it = vectorWorkSizeData.constBegin();
            it != vectorWorkSizeData.constEnd(); ++it)
    {
        _vectorWorkSizeData.append(QPointF(it.key(), it.value()));
        ui.workSizeTable->setItem(i, 1,
                new QTableWidgetItem(QString::number(it.value())));
        ++i;
    }
    _vectorWorkSizeCurve->setData(_vectorWorkSizeData);

    _singleDataData.clear();
    header.clear();
    i = 0;
    for (it = singleDataData.constBegin(); it != singleDataData.constEnd();
            ++it)
    {
        header.append(QString::number(it.key()));
        ui.dataTable->setItem(i, 0,
                new QTableWidgetItem(QString::number(it.value())));
        _singleDataData.append(QPointF(it.key(), it.value()));
        ++i;
    }
    _singleDataCurve->setData(_singleDataData);
    ui.dataTable->setVerticalHeaderLabels(header);

    _vectorDataData.clear();
    i = 0;
    for (it = vectorDataData.constBegin();
            it != vectorDataData.constEnd(); ++it)
    {
        ui.dataTable->setItem(i, 1,
                new QTableWidgetItem(QString::number(it.value())));
        _vectorDataData.append(QPointF(it.key(), it.value()));
        ++i;
    }
    _vectorDataCurve->setData(_vectorDataData);

    ui.dataPlot->replot();
    ui.workSizePlot->replot();
}
