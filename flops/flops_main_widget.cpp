#include "flops_main_widget.h"
#include <qwt_scale_engine.h>
#include <QPen>
#include <QBrush>

FlopsMainWidget::FlopsMainWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.workSizeFlopsPlot->setAxisTitle(0, "Flops");
    ui.workSizeFlopsPlot->setAxisTitle(2, "Workgroup size");
    ui.workSizeFlopsPlot->setAxisScaleEngine(2, new QwtLog10ScaleEngine());
    ui.workSizeFlopsPlot->setAxisScaleEngine(0, new QwtLog10ScaleEngine());

    ui.dataFlopsPlot->setAxisTitle(0, "Flops");
    ui.dataFlopsPlot->setAxisTitle(2, "Data");
    ui.dataFlopsPlot->setAxisScaleEngine(2, new QwtLog10ScaleEngine());
    ui.dataFlopsPlot->setAxisScaleEngine(0, new QwtLog10ScaleEngine());

    _workSizeCurve = new QwtPlotCurve("curve");
    _workSizeCurve->setData(_workSizeData);
    _workSizeCurve->attach(ui.workSizeFlopsPlot);

    _workSizeVector4Curve = new QwtPlotCurve("vector4curve");
    _workSizeVector4Curve->setData(_workSizeVector4Data);
    _workSizeVector4Curve->attach(ui.workSizeFlopsPlot);

    _dataCurve = new QwtPlotCurve("curve");
    _dataCurve->setData(_dataData);
    _dataCurve->attach(ui.dataFlopsPlot);

    _dataVector4Curve = new QwtPlotCurve("vector4curve");
    _dataVector4Curve->setData(_dataVector4Data);
    _dataVector4Curve->attach(ui.dataFlopsPlot);

    _workSizeCurve->setPen(QPen(QBrush(Qt::red), 3.0));
    _workSizeVector4Curve->setPen(QPen(QBrush(Qt::blue), 3.0));

    _dataCurve->setPen(QPen(QBrush(Qt::red), 3.0));
    _dataVector4Curve->setPen(QPen(QBrush(Qt::blue), 3.0));
}

FlopsMainWidget::~FlopsMainWidget()
{
    delete _workSizeCurve;
    delete _workSizeVector4Curve;
    delete _dataVector4Curve;
    delete _dataCurve;
}

void FlopsMainWidget::showResults(QMap<size_t, double> &workSizeData,
        QMap<size_t, double> &dataData,
        QMap<size_t, double> &workSizeVector4Data,
        QMap<size_t, double> &dataVector4Data)
{
    QMap<size_t, double>::const_iterator it;

    _workSizeData.clear();
    for (it = workSizeData.constBegin(); it != workSizeData.constEnd(); ++it)
        _workSizeData.append(QPointF(it.key(), it.value()));
    _workSizeCurve->setData(_workSizeData);

    _workSizeVector4Data.clear();
    for (it = workSizeVector4Data.constBegin();
            it != workSizeVector4Data.constEnd(); ++it)
        _workSizeVector4Data.append(QPointF(it.key(), it.value()));
    _workSizeVector4Curve->setData(_workSizeVector4Data);

    _dataData.clear();
    for (it = dataData.constBegin(); it != dataData.constEnd(); ++it)
        _dataData.append(QPointF(it.key(), it.value()));
    _dataCurve->setData(_dataData);

    _dataVector4Data.clear();
    for (it = dataVector4Data.constBegin();
            it != dataVector4Data.constEnd(); ++it)
        _dataVector4Data.append(QPointF(it.key(), it.value()));
    _dataVector4Curve->setData(_dataVector4Data);

    ui.dataFlopsPlot->replot();
    ui.workSizeFlopsPlot->replot();
}
