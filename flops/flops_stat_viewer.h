#ifndef FLOPS_STATS_VIEWER_H
#define FLOPS_STATS_VIEWER_H
#include <QWidget>
#include <QMap>
#include <QPolygon>
#include <qwt_plot_curve.h>
#include "ui_flops_stat_viewer.h"

class FlopsStatViewer
    : public QWidget
{
    Q_OBJECT

    public:
        FlopsStatViewer(QWidget *parent = 0);
        ~FlopsStatViewer();

        void showResults(
                QMap<size_t, double> &singleWorkSizeData,
                QMap<size_t, double> &singleDataData,
                QMap<size_t, double> &vectorWorkSizeData,
                QMap<size_t, double> &vectorDataData);
        void setWorkSizeProgress(int progress);
        void setDataProgress(int progress);

    private:
        Ui::FlopsStatViewer ui;

        QwtPlotCurve *_singleWorkSizeCurve;
        QPolygonF _singleWorkSizeData;

        QwtPlotCurve *_vectorWorkSizeCurve;
        QPolygonF _vectorWorkSizeData;

        QwtPlotCurve *_singleDataCurve;
        QPolygonF _singleDataData;

        QwtPlotCurve *_vectorDataCurve;
        QPolygonF _vectorDataData;

};
#endif // FLOPS_STATS_VIEWER_H
