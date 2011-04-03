#ifndef MANDELBROT_CONFIG_WIDGET_H
#define MANDELBROT_CONFIG_WIDGET_H
#include <QWidget>
#include "ui_mandelbrot_config_widget.h"

class MandelbrotConfigWidget
    : public QWidget
{
    Q_OBJECT

    public:
        MandelbrotConfigWidget(QWidget *parent = 0);
        virtual ~MandelbrotConfigWidget();

        int getMaxIterations() const;
        bool useDouble() const;

        void setResolution(int x, int y);
        void setRenderTimeCPU(double sec);
        void setRenderTimeOpenMP(double sec);
        void setRenderTimeOpenCL(double sec);
        void supportDouble(bool supportDouble);

        bool useCpu();
        bool useOpenMP();

    public slots:
        void setRunning(bool isRunning);
        void updateFixedSize();

    signals:
        void maxIterationsChanged(int iterations);
        void recalculate();
        void setFixedSize(bool isFixed, int width, int height);

    private:
        Ui::MandelbrotConfigWidget ui;
};


#endif // MANDELBROT_CONFIG_WIDGET_H
