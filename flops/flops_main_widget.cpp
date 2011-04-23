#include "flops_main_widget.h"

#include <qwt_scale_engine.h>
#include <QPen>
#include <QBrush>

FlopsMainWidget::FlopsMainWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    
    for (int i = 0; i < ui.tabWidget->count(); ++i)
    {
        QWidget *widget = ui.tabWidget->widget(i);
        FlopsStatViewer *statViewer = new FlopsStatViewer(this);
        widget->setLayout(new QHBoxLayout);
        widget->layout()->addWidget(statViewer);
        _statViewers.append(statViewer);
    }

    _currentDataType = ui.tabWidget->currentIndex();
}

FlopsMainWidget::~FlopsMainWidget()
{
}

void FlopsMainWidget::setCurrentDataType()
{
    int index = ui.tabWidget->currentIndex();
    if (ui.tabWidget->widget(index))
        _currentDataType = index;
}

int FlopsMainWidget::getSelectedTab()
{
    return ui.tabWidget->currentIndex();
}

void FlopsMainWidget::showResults(
        QMap<size_t, double> &singleWorkSizeData,
        QMap<size_t, double> &singleDataData,
        QMap<size_t, double> &vectorWorkSizeData,
        QMap<size_t, double> &vectorDataData)
{
    _statViewers[_currentDataType]->showResults(
            singleWorkSizeData, singleDataData,
            vectorWorkSizeData, vectorDataData);
}

void FlopsMainWidget::setWorkSizeProgress(int progress)
{
    _statViewers[_currentDataType]->setWorkSizeProgress(progress);
}

void FlopsMainWidget::setDataProgress(int progress)
{
    _statViewers[_currentDataType]->setDataProgress(progress);
}

QString FlopsMainWidget::getDataType()
{
    switch (_currentDataType)
    {
        case FLOAT:
            return "float";
        case DOUBLE:
            return "double";
        case INTEGER:
            return "int";
        case HALF:
            return "half";
    }
}

int FlopsMainWidget::getDataTypeId()
{
    return _currentDataType;
}
