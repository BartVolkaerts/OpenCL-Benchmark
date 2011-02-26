#include "flops_config_widget.h"

FlopsConfigWidget::FlopsConfigWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

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
