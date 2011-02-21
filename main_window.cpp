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

    _benchmarks.insert(FlopsBenchmark::getName(),
            new FlopsBenchmark(_environment, this));

    ui.benchmarkList->addItems(_benchmarks.keys());
    ui.centralwidget->setLayout(new QVBoxLayout());

    setBenchmarkWidgets(FlopsBenchmark::getName());

    connect(ui.benchmarkList, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(setBenchmarkWidgets(const QString &)));
    connect(ui.startButton, SIGNAL(clicked()),
            this, SLOT(launchBenchmark()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::setBenchmarkWidgets(const QString &benchmark)
{
    ui.benchmarkWidget->setWidget(_benchmarks[benchmark]->getConfigWidget());
    QLayoutItem *currentWidget = ui.centralwidget->layout()->itemAt(0);
    if (currentWidget)
        ui.centralwidget->layout()->removeItem(currentWidget);
    ui.centralwidget->layout()->addWidget(
            _benchmarks[benchmark]->getMainWidget());
}

void MainWindow::launchBenchmark()
{
    BaseBenchmark *benchmark = getSelectedBenchmark();
    if (benchmark)
        benchmark->execute();
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

BaseBenchmark *MainWindow::getSelectedBenchmark()
{
    QListWidgetItem *currentItem = ui.benchmarkList->currentItem();
    if (currentItem)
        return _benchmarks[currentItem->text()];
    else
        return NULL;
}
