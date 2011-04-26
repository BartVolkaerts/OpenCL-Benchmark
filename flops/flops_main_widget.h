#ifndef FLOPS_MAIN_WIDGET_H
#define FLOPS_MAIN_WIDGET_H
#include <QWidget>
#include <QMap>
#include <QPolygonF>
#include <QVector>
#include <QString>
#include "ui_flops_main_widget.h"
#include "flops_stat_viewer.h"

class FlopsMainWidget
    : public QWidget
{
    Q_OBJECT

    public:
        FlopsMainWidget(QWidget *parent = 0);
        virtual ~FlopsMainWidget();

        void setCurrentDataType();
        int getSelectedTab();

        void showResults(
                QMap<size_t, double> &singleWorkSizeData,
                QMap<size_t, double> &singleDataData,
                QMap<size_t, double> &vectorWorkSizeData,
                QMap<size_t, double> &vectorDataData);
        void setWorkSizeProgress(int progress);
        void setDataProgress(int progress);
        QString getDataType();
        int getDataTypeId();
        void setDoubleSupport(bool isSupported);

        enum {
            FLOAT = 0,
            INTEGER = 1,
            DOUBLE = 2,
            HALF = 3
            };

    private:
        Ui::FlopsMainWidget ui;

        int _currentDataType;
        QVector<FlopsStatViewer *> _statViewers;
};

#endif // FLOPS_MAIN_WIDGET_H
