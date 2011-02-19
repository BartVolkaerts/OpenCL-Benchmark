#include "main_window.h"
#include "environment.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    _environment = new Environment(this);

    _platforms = _environment->getPlatformsMap();
    setPlatformBox();
    connect(ui.platformBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(platformBoxChanged(const QString &)));

    connect(ui.deviceBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(deviceBoxChanged(const QString &)));

    //_environment->createContext();
    //_environment->createCommandQueue();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setDevicesBox()
{
    qDebug() << 1;
    ui.deviceBox->clear();
    QString currentText(ui.deviceBox->currentText());
    ui.deviceBox->addItems(_devices.keys());
    deviceBoxChanged(currentText);
    qDebug() << currentText;
}

void MainWindow::setPlatformBox()
{
    qDebug() << 2;
    ui.platformBox->clear();
    QString currentText(ui.platformBox->currentText());
    ui.platformBox->addItems(_platforms.keys());
    platformBoxChanged(_platforms.keys()[0]);
    qDebug() << _platforms.keys()[0];
}

void MainWindow::platformBoxChanged(const QString &currentIndex)
{
    qDebug() << 3;
    _environment->setPlatform(_platforms[currentIndex]);
    _devices = _environment->getDevicesMap();
    setDevicesBox();
}

void MainWindow::deviceBoxChanged(const QString &currentIndex)
{
    qDebug() << 4;
    _environment->setDevice(_devices[currentIndex]);
}
