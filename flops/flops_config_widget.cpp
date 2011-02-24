#include "flops_config_widget.h"

FlopsConfigWidget::FlopsConfigWidget(int max, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.dataMaxDataBox->setMaximum(max);
    ui.dataMaxDataBox->setValue(max / 2);

    ui.dataStartAtBox->setMaximum(max);
    ui.workGroupDataBox->setMaximum(max);

}

FlopsConfigWidget::~FlopsConfigWidget()
{

}

int FlopsConfigWidget::getDataMaxData()
{
    return ui.dataMaxDataBox->value();
}

int FlopsConfigWidget::getDataStartData()
{
    return ui.dataStartAtBox->value();
}

int FlopsConfigWidget::getWorkSizeAmountOfData()
{
    return ui.workGroupDataBox->value();
}
