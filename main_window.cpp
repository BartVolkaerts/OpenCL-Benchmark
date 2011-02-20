#include "main_window.h"

#include <QVBoxLayout>

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

    _environment->createContext();
    _environment->createCommandQueue();
    _benchmark = new FlopsBenchmark(_environment, this);

    ui.benchmarkWidget->setWidget(_benchmark->getConfigWidget());
    ui.centralwidget->setLayout(new QVBoxLayout());
    ui.centralwidget->layout()->addWidget(_benchmark->getMainWidget());

    connect(ui.startButton, SIGNAL(clicked()),
            _benchmark, SLOT(execute()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::setDevicesBox()
{
    ui.deviceBox->clear();
    QString currentText(ui.deviceBox->currentText());
    ui.deviceBox->addItems(_devices.keys());
    deviceBoxChanged(currentText);
}

void MainWindow::setPlatformBox()
{
    ui.platformBox->clear();
    QString currentText(ui.platformBox->currentText());
    ui.platformBox->addItems(_platforms.keys());
    platformBoxChanged(_platforms.keys()[0]);
}

void MainWindow::platformBoxChanged(const QString &currentIndex)
{
    _environment->setPlatform(_platforms[currentIndex]);
    _devices = _environment->getDevicesMap();
    setDevicesBox();
}

void MainWindow::deviceBoxChanged(const QString &)
{
    _environment->setDevice(_devices[ui.deviceBox->currentText()]);
}
