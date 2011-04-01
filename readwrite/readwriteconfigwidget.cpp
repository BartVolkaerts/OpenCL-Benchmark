#include "readwriteconfigwidget.h"
#include "ui_readwriteconfigwidget.h"

#include <QDebug>

ReadWriteConfigWidget::ReadWriteConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReadWriteConfigWidget)
{
    ui->setupUi(this);
    connect(ui->checkBox, SIGNAL(toggled(bool)), ui->groupBox, SLOT(setDisabled(bool)));
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(changeCaptureDevice(bool)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(getFile()));

    ui->lineEdit->setEnabled(false);
}

ReadWriteConfigWidget::~ReadWriteConfigWidget()
{
    delete ui;
}

void ReadWriteConfigWidget::changeCaptureDevice(bool state)
{
    if (state==true)
        ui->lineEdit->setText("");
    emit changedevice(state);
}

void ReadWriteConfigWidget::getFile()
{
    QString temp = QFileDialog::getOpenFileName(this,
         tr("Open Image"), "/home/", tr(""));
    if (temp != "")
    {
        ui->lineEdit->setText(temp);
        emit fileName(ui->lineEdit->text());
    }
}

void ReadWriteConfigWidget::setCPUTime(double time)
{
    ui->cpuTimelbl->setText(QString::number(time));
}

void ReadWriteConfigWidget::setGPUTime(double time)
{
    ui->gpuTimelbl->setText(QString::number(time));
}

void ReadWriteConfigWidget::setLocked()
{
    ui->checkBox->setDisabled(true);
    ui->groupBox->setDisabled(true);
}

void ReadWriteConfigWidget::setUnlocked()
{
    ui->checkBox->setDisabled(false);
    if(ui->checkBox->isChecked())
    {
        ui->groupBox->setDisabled(true);
    }
    else
    {
        ui->groupBox->setDisabled(false);
    }

}
