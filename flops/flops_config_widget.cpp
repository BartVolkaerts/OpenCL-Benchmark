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

QString FlopsConfigWidget::getKernelType()
{
    if (ui.addButton->isChecked())
        return "addition";
    else if (ui.divButton->isChecked())
        return "divide";
    else if (ui.madButton->isChecked())
        return "muladd";
    else
        return "multiply";
}
