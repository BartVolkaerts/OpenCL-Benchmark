#include "main_window.h"

#include <QVBoxLayout>
#include <QMessageBox>

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

    addBenchmark(FlopsBenchmark::getName(),
            new FlopsBenchmark(_environment, this));
    addBenchmark(ReadWrite::getName(),
            new ReadWrite(_environment, this));
    addBenchmark(Galaxy::getName(),
            new Galaxy(_environment, this));
    addBenchmark(Mandelbrot::getName(),
            new Mandelbrot(_environment, this));
    addBenchmark(Raytracing::getName(),
            new Raytracing(_environment, this));

    ui.benchmarkList->addItems(_benchmarks.keys());
    ui.centralwidget->setLayout(new QVBoxLayout());
    ui.centralwidget->layout()->setAlignment(Qt::AlignHCenter);

    connect(ui.benchmarkList, SIGNAL(currentTextChanged(const QString &)),
            this, SLOT(setBenchmarkWidgets(const QString &)));
    connect(ui.startButton, SIGNAL(clicked()),
            this, SLOT(launchBenchmark()));
    connect(ui.stopButton, SIGNAL(clicked()),
            this, SLOT(stopBenchmark()));
    connect(ui.actionAbout, SIGNAL(triggered()),
            this, SLOT(showAbout()));
    connect(ui.actionQuit, SIGNAL(triggered()),
            this, SLOT(close()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::addBenchmark(const QString &name, BaseBenchmark *benchmark)
{
    _benchmarks.insert(name, benchmark);
    benchmark->getMainWidget()->setVisible(false);
    benchmark->getConfigWidget()->setVisible(false);
    benchmark->getMainWidget()->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Expanding);
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
    if (!benchmark)
        return;
    setBenchmarkRunningLock(true);
    benchmark->execute();
    if (!benchmark->waitForStop())
        setBenchmarkRunningLock(false);
}

void MainWindow::stopBenchmark()
{
    if (_currentBenchmark->waitForStop())
    {
        _currentBenchmark->stop();
        setBenchmarkRunningLock(false);
    }
}

void MainWindow::setBenchmarkRunningLock(bool locked)
{
#if 0
    if (locked)
        QApplication::setOverrideCursor(Qt::WaitCursor);
    else
        QApplication::restoreOverrideCursor();
#endif

    ui.startButton->setEnabled(!locked);
    ui.stopButton->setEnabled(locked);
    ui.benchmarkList->setEnabled(!locked);
    ui.hardwareWidget->setEnabled(!locked);
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

void MainWindow::showAbout()
{
    QString aboutMessage(
            "<h1>About this benchmark</h1>"
            "<p>This program is written as part of or final project at the "
            "denayer instituut in Sint -Katelijne Waver.<p>"
            "<h2>Authors:</h2>"
            "<ul>"
            "<li>Bart Volkaers</li>"
            "<li>Wiebe Van Ranst - wiebevr@gmail.com</li>"
            "</ul>");

    QMessageBox::about(this, "About this benchmark", aboutMessage);
}
