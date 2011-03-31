#include "io_throughput_config_widget.h"

IoThroughputConfigWidget::IoThroughputConfigWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.minDataBox, SIGNAL(valueChanged(int)),
            this, SLOT(setMinData(int)));
    connect(ui.maxDataBox, SIGNAL(valueChanged(int)),
            this, SLOT(setMaxData(int)));
}

IoThroughputConfigWidget::~IoThroughputConfigWidget()
{

}

int IoThroughputConfigWidget::getMaxData()
{
    return ui.maxDataBox->value();
}

int IoThroughputConfigWidget::getMinData()
{
    return ui.minDataBox->value();
}

void IoThroughputConfigWidget::setMinData(int min)
{
    ui.maxDataBox->setMinimum(min);
}

void IoThroughputConfigWidget::setMaxData(int max)
{
    ui.minDataBox->setMaximum(max);
}
