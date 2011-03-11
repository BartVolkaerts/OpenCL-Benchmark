#include <QString>
#include "mandelbrot_config_widget.h"

MandelbrotConfigWidget::MandelbrotConfigWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.dataTypeComboBox->addItem("Single precision float");
    connect(ui.iterationsSpinBox, SIGNAL(valueChanged(int)),
            this, SIGNAL(maxIterationsChanged(int)));
    connect(ui.recalculateButton, SIGNAL(clicked()),
            this, SIGNAL(recalculate()));
    connect(ui.fixedSizeToggleButton, SIGNAL(clicked()),
            this, SLOT(updateFixedSize()));
    connect(ui.setWindowWidthSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateFixedSize()));
    connect(ui.setWindowHeightSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(updateFixedSize()));
}

MandelbrotConfigWidget::~MandelbrotConfigWidget()
{
}

int MandelbrotConfigWidget::getMaxIterations() const
{

    return ui.iterationsSpinBox->value();
}

bool MandelbrotConfigWidget::useDouble() const
{

    return ui.dataTypeComboBox->currentIndex() == 1;
}

void MandelbrotConfigWidget::supportDouble(bool supportDouble)
{
    if (supportDouble && ui.dataTypeComboBox->count() <= 1)
        ui.dataTypeComboBox->addItem("Double precision float");
}

void MandelbrotConfigWidget::setResolution(int x, int y)
{
    ui.resolutionValueLabel->setText(QString("%1 x %2").arg(x).arg(y));
}

void MandelbrotConfigWidget::setRenderTime(double sec)
{
    ui.renderTimeValueLabel->setText(QString("%1 ms").arg(sec * 1000.0));
}

void MandelbrotConfigWidget::setRunning(bool isRunning)
{
    ui.dataTypeComboBox->setEnabled(!isRunning);
    ui.recalculateButton->setEnabled(isRunning);
}

void MandelbrotConfigWidget::updateFixedSize()
{
    emit setFixedSize(ui.fixedSizeToggleButton->isChecked(),
            ui.setWindowWidthSpinBox->value(),
            ui.setWindowHeightSpinBox->value());
}
