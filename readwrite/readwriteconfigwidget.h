#ifndef READWRITECONFIGWIDGET_H
#define READWRITECONFIGWIDGET_H

#include <QWidget>
#include <QFileDialog>

namespace Ui {
    class ReadWriteConfigWidget;
}

class ReadWriteConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReadWriteConfigWidget(QWidget *parent = 0);
    ~ReadWriteConfigWidget();

    void setOpenCLTime(double);
    void setLocked();
    void setUnlocked();

    bool copyImage();
    bool useEdgeDetection();
    bool useSharpening();

public slots:
    void changeCaptureDevice(bool);
    void getFile();

signals:
    void changedevice(bool);
    void fileName(QString);

private:
    Ui::ReadWriteConfigWidget *ui;
};

#endif // READWRITECONFIGWIDGET_H
