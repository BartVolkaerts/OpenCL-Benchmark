#ifndef FLOPS_CONFIG_WIDGET_H
#define FLOPS_CONFIG_WIDGET_H
#include <QWidget>
#include "ui_flops_config_widget.h"

class FlopsConfigWidget
    : public QWidget
{
    Q_OBJECT

    public:
        FlopsConfigWidget(QWidget *parent = 0);
        virtual ~FlopsConfigWidget();

        int getDataMaxData();
        int getDataStartData();
        int getWorkSizeAmountOfData();

    private:
        Ui::FlopsConfigWidget ui;

};

#endif // FLOPS_CONFIG_WIDGET_H
