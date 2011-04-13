#include "readwriteconfigwidget.h"
#include "ui_readwriteconfigwidget.h"

#include <QDebug>

ReadWriteConfigWidget::ReadWriteConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReadWriteConfigWidget)
{
    ui->setupUi(this);
    connect(ui->checkBox, SIGNAL(toggled(bool)), ui->pushButton, SLOT(setDisabled(bool)));
    connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(changeCaptureDevice(bool)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(getFile()));

    ui->lineEdit->setEnabled(false);
    ui->copyButton->setChecked(true);
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

void ReadWriteConfigWidget::setOpenCLTime(double time)
{
    ui->openCLTimeLbl->setText(QString::number(time));
}

void ReadWriteConfigWidget::setResolution(QString res)
{
    ui->resLbl->setText(res);
}

void ReadWriteConfigWidget::setFramerate(QString framerate)
{
    ui->frameRlbl->setText(framerate);
}

void ReadWriteConfigWidget::setLocked()
{
    ui->groupBox->setDisabled(true);
    _timerId = startTimer(1000);
}

void ReadWriteConfigWidget::setUnlocked()
{
    ui->groupBox->setDisabled(false);
    killTimer(_timerId);
}

bool ReadWriteConfigWidget::copyImage()
{
    return ui->copyButton->isChecked();
}

bool ReadWriteConfigWidget::useEdgeDetection()
{
    return ui->edgeButton->isChecked();
}

bool ReadWriteConfigWidget::useSharpening()
{
    return ui->sharpButton->isChecked();
}

void ReadWriteConfigWidget::addFrame()
{
    _renderedFrames++;
}

void ReadWriteConfigWidget::timerEvent(QTimerEvent *)
{
    ui->frameRlbl->setText(QString::number(_renderedFrames));
    _renderedFrames = 0;
}
