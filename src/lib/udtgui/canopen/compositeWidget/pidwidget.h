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

#ifndef PIDWIDGET_H
#define PIDWIDGET_H

#include "../../udtgui_global.h"

#include <QWidget>

#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTimer>
#include <QToolBar>

class Node;
class DataLogger;
class DataLoggerWidget;
class IndexSpinBox;
class IndexLabel;
class IndexCheckBox;

class UDTGUI_EXPORT PidWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PidWidget(QWidget *parent = nullptr);

    Node *node() const;
    QString title() const;

    enum ModePid
    {
        MODE_PID_NONE,
        MODE_PID_VELOCITY,
        MODE_PID_POSITION,
        MODE_PID_TORQUE,
    };

public slots:
    void setNode(Node *node, uint8_t axis = 0);
    void setMode(PidWidget::ModePid mode);

protected slots:
    void toggleStartLogger(bool start);
    void setLogTimer(int ms);

protected:
    uint8_t _axis;
    NodeProfile402 *_nodeProfile402;
    ModePid _modePid;

    QTimer _timer;
    QTimer _readStatusTimer;
    DataLogger *_dataLogger;

    NodeObjectId _actualValue_ObjId;

    void createWidgets();
    QToolBar *createToolBarWidgets();
    QToolBar *_toolBar;
    QSpinBox *_logTimerSpinBox;

    DataLoggerWidget *_dataLoggerWidget;

    QGroupBox *createPIDConfigWidgets();
    QGroupBox *_pidGroupBox;
    IndexSpinBox *_pSpinBox;
    IndexSpinBox *_iSpinBox;
    IndexSpinBox *_dSpinBox;
    IndexSpinBox *_minSpinBox;
    IndexSpinBox *_maxSpinBox;
    IndexSpinBox *_thresholdSpinBox;
    IndexSpinBox *_freqDividerSpinBox;
    IndexCheckBox *_antiReverseCheckBox;
    IndexCheckBox *_directCtrlCheckBox;

    QGroupBox *createPIDStatusWidgets();
    QGroupBox *_pidStatusGroupBox;
    IndexLabel *_inputLabel;
    IndexLabel *_errorLabel;
    IndexLabel *_integratorLabel;
    IndexLabel *_outputLabel;

    QGroupBox *createPIDTestWidgets();
    QGroupBox *_pidTestGroupBox;
    QSpinBox *_firstTargetSpinBox;
    QSpinBox *_secondTargetSpinBox;
    QSpinBox *_windowFirstTargetSpinBox;
    QSpinBox *_windowSecondTargetSpinBox;
    QSpinBox *_stopDataLoggerSpinBox;
    QPushButton *_stopTargetPushButton;
    QPushButton *_startTargetPushButton;
    QPushButton *_savePushButton;

    enum State
    {
        NONE,
        LAUCH_DATALOGGER,
        LAUCH_FIRST_TARGET,
        LAUCH_SECOND_TARGET,
        STOP_DATALOGGER,
    };
    State _state;

    void setIMode();
    void screenshotSave();
    void changeMode402();
    void mode402Changed(uint8_t axis, NodeProfile402::OperationMode mode);
    void manageMeasurement();
    void stopFirstMeasurement();
    void stopSecondMeasurement();
    void stopMeasurement();
    void stopDataLogger();
    void readStatus();
    void readAllObject();
    void statusNodeChanged(Node::Status status);
};

#endif  // PIDWIDGET_H
