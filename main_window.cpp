#include "main_window.h"

#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    _environment = new Environment(this);
    _currentBenchmark = NULL;

    _platforms = _environment->getPlatformsMap();
    setPlatformBox();
    connect(ui.platformBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(platformBoxChanged(const QString &)));

    connect(ui.deviceBox, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(deviceBoxChanged(const QString &)));

    _environment->createContext();
    _environment->createCommandQueue();

    addBenchmark(FlopsBenchmark::getName(),
            new FlopsBenchmark(_environment, this));
    addBenchmark("Flops2",
            new FlopsBenchmark(_environment, this));
    addBenchmark(Read_Write_Benchmark::getName(),
            new Read_Write_Benchmark(_environment, this));

    ui.benchmarkList->addItems(_benchmarks.keys());
    ui.centralwidget->setLayout(new QVBoxLayout());

    //setBenchmarkWidgets(FlopsBenchmark::getName());

    connect(ui.benchmarkList, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(setBenchmarkWidgets(const QString &)));
    connect(ui.startButton, SIGNAL(clicked()),
            this, SLOT(launchBenchmark()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::addBenchmark(const QString &name, BaseBenchmark *benchmark)
{
    _benchmarks.insert(name, benchmark);
    benchmark->getMainWidget()->setVisible(false);
    benchmark->getConfigWidget()->setVisible(false);
}

void MainWindow::setBenchmarkWidgets(const QString &benchmark)
{
    // Make previous invisible
    if (_currentBenchmark)
    {
        _currentBenchmark->getMainWidget()->setVisible(false);
        _currentBenchmark->getConfigWidget()->setVisible(false);
    }

    // Set current benchmark
    _currentBenchmark = _benchmarks[benchmark];
    ui.benchmarkWidget->setWidget(_benchmarks[benchmark]->getConfigWidget());

    // Remove previous widget
    QLayoutItem *currentWidget = ui.centralwidget->layout()->itemAt(0);
    if (currentWidget)
    {
        ui.centralwidget->layout()->removeItem(currentWidget);
    }
    ui.centralwidget->layout()->addWidget(
            _benchmarks[benchmark]->getMainWidget());

    _currentBenchmark->getMainWidget()->setVisible(true);
    _currentBenchmark->getConfigWidget()->setVisible(true);
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
        return _currentBenchmark;
}
