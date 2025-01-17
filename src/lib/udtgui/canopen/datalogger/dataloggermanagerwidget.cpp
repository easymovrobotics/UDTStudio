/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#include "dataloggermanagerwidget.h"

#include <QHBoxLayout>

DataLoggerManagerWidget::DataLoggerManagerWidget(DataLogger *logger, QWidget *parent)
    : QWidget(parent)
    , _logger(logger)
{
    createWidgets();
    _chartWidget = nullptr;
}

void DataLoggerManagerWidget::toggleStartLogger(bool start)
{
    if (start)
    {
        _logger->start(_logTimerSpinBox->value());
    }
    else
    {
        _logger->stop();
    }
}

void DataLoggerManagerWidget::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _logger->start(ms);
    }
}

void DataLoggerManagerWidget::setUseOpenGL(bool useOpenGL)
{
    if (_chartWidget)
    {
        _chartWidget->setUseOpenGL(useOpenGL);
    }
    _crossAction->setEnabled(!useOpenGL);
}

void DataLoggerManagerWidget::setViewCross(bool viewCross)
{
    if (_chartWidget)
    {
        _chartWidget->setViewCross(viewCross);
    }
}

DataLoggerChartsWidget *DataLoggerManagerWidget::chartWidget() const
{
    return _chartWidget;
}

void DataLoggerManagerWidget::setChartWidget(DataLoggerChartsWidget *chartWidget)
{
    _chartWidget = chartWidget;
}

void DataLoggerManagerWidget::createWidgets()
{
    QAction *action;
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // toolbar nmt
    _toolBar = new QToolBar(tr("Data logger commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // start stop
    _startStopAction = _toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    QIcon iconStartStop;
    iconStartStop.addFile(":/icons/img/icons8-stop.png", QSize(), QIcon::Normal, QIcon::On);
    iconStartStop.addFile(":/icons/img/icons8-play.png", QSize(), QIcon::Normal, QIcon::Off);
    _startStopAction->setIcon(iconStartStop);
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &DataLoggerManagerWidget::toggleStartLogger);
    connect(_logger, &DataLogger::startChanged, [=] (bool changed)
    {
        if (changed != _startStopAction->isChecked())
        {
            _startStopAction->blockSignals(true);
            _startStopAction->setChecked(changed);
            _startStopAction->blockSignals(false);
        }
    });

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setStatusTip(tr("Sets the interval of log timer in ms"));
    _toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setLogTimer(i); });

    // clear
    action = _toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    action->setStatusTip(tr("Clear all data"));
    connect(action, &QAction::triggered, _logger, &DataLogger::clear);

    _toolBar->addSeparator();

    // speed
    _openGLAction = _toolBar->addAction(tr("Open-GL"));
    _openGLAction->setCheckable(true);
    _openGLAction->setChecked(false);
    _openGLAction->setIcon(QIcon(":/icons/img/icons8-speed.png"));
    _openGLAction->setStatusTip(tr("Sets render to open GL for fast rendering"));
    connect(_openGLAction, &QAction::triggered, this, &DataLoggerManagerWidget::setUseOpenGL);

    // linechart
    _crossAction = _toolBar->addAction(tr("Cross"));
    _crossAction->setCheckable(true);
    _crossAction->setEnabled(true);
    _crossAction->setIcon(QIcon(":/icons/img/icons8-line-chart.png"));
    _crossAction->setStatusTip(tr("Adds cross to line chart"));
    connect(_crossAction, &QAction::triggered, this, &DataLoggerManagerWidget::setViewCross);

    layout->addWidget(_toolBar);

    _dataLoggerTreeView = new DataLoggerTreeView();
    _dataLoggerTreeView->setDataLogger(_logger);
    layout->addWidget(_dataLoggerTreeView);

    setLayout(layout);
}

QAction *DataLoggerManagerWidget::startStopAction() const
{
    return _startStopAction;
}
