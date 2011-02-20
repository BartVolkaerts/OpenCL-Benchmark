#ifndef FLOPS_MAIN_WIDGET_H
#define FLOPS_MAIN_WIDGET_H
#include <QWidget>
#include <QMap>
#include <QPolygonF>
#include <qwt_plot_curve.h>
#include "ui_flops_main_widget.h"

class FlopsMainWidget
    : public QWidget
{
    Q_OBJECT

    public:
        FlopsMainWidget(QWidget *parent = 0);
        virtual ~FlopsMainWidget();

        void showResults(QMap<size_t, double> data);

    private:
        Ui::FlopsMainWidget ui;

        QwtPlotCurve *_curve;
        QPolygonF _data;
};

#endif // FLOPS_MAIN_WIDGET_H
