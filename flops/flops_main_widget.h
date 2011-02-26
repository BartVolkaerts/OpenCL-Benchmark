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

        void showResults(QMap<size_t, double> &workSizeData,
                QMap<size_t, double> &dataData,
                QMap<size_t, double> &workSizeVector4Data,
                QMap<size_t, double> &dataVector4Data);
        void setWorkSizeProgress(int progress);
        void setDataProgress(int progress);

    private:
        Ui::FlopsMainWidget ui;

        QwtPlotCurve *_workSizeCurve;
        QPolygonF _workSizeData;

        QwtPlotCurve *_workSizeVector4Curve;
        QPolygonF _workSizeVector4Data;

        QwtPlotCurve *_dataCurve;
        QPolygonF _dataData;

        QwtPlotCurve *_dataVector4Curve;
        QPolygonF _dataVector4Data;
};

#endif // FLOPS_MAIN_WIDGET_H
