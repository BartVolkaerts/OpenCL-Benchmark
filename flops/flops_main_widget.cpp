#include "flops_main_widget.h"

FlopsMainWidget::FlopsMainWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    _data.append(QPointF(0.0, 0.0));

    _curve = new QwtPlotCurve("curve");
    _curve->setData(_data);
    _curve->attach(ui.workSizeTimePlot);
    //ui.workSizeTimePlot->replot();
}

FlopsMainWidget::~FlopsMainWidget()
{
    delete _curve;
}

void FlopsMainWidget::showResults(QMap<size_t, double> data)
{
    _data.clear();
    QMap<size_t, double>::const_iterator it;
    for (it = data.constBegin(); it != data.constEnd(); ++it)
        _data.append(QPointF(it.key(), it.value()));
    _curve->setData(_data);

    ui.workSizeTimePlot->replot();
}
