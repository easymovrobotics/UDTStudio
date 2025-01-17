﻿/**
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

#include "p402widget.h"

#include "canopen/indexWidget/indexlabel.h"
#include "services/services.h"

#include "p402cpwidget.h"
#include "p402dtywidget.h"
#include "p402ipwidget.h"
#include "p402optionwidget.h"
#include "p402ppwidget.h"
#include "p402tqwidget.h"
#include "p402vlwidget.h"

#include <QApplication>
#include <QButtonGroup>
#include <QDebug>
#include <QFormLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>

#include "canopen/datalogger/dataloggerwidget.h"

P402Widget::P402Widget(Node *node, uint8_t axis, QWidget *parent) : QWidget(parent), _node(node), _axis(axis)
{
    createWidgets();

    if (!_node)
    {
        return;
    }

    if ((node->profileNumber()) != 0x192)
    {
        return;
    }

    setNode(node, axis);
}

P402Widget::~P402Widget()
{
}

Node *P402Widget::node() const
{
    return _node;
}

QString P402Widget::title() const
{
    return QString("Motion Control");
}

void P402Widget::setNode(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }
    _node = node;

    if ((node->profileNumber() != 0x192) || node->profiles().isEmpty())
    {
        return;
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    if (_node)
    {
        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
        _nodeProfile402->init();

        _controlWordObjectId = _nodeProfile402->controlWordObjectId();
        _controlWordLabel->setObjId(_controlWordObjectId);

        _statusWordObjectId = _nodeProfile402->statusWordObjectId();
        _statusWordLabel->setObjId(_statusWordObjectId);

        setCheckableStateMachine(2);
        updateModeComboBox();

        connect(_modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int id) { setModeIndex(id); });
        connect(_nodeProfile402, &NodeProfile402::modeChanged, this, &P402Widget::updateMode);
        connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &P402Widget::updateState);
        connect(_nodeProfile402, &NodeProfile402::isHalted, _haltPushButton, &QPushButton::setChecked);
        connect(_nodeProfile402, &NodeProfile402::eventHappened, this, &P402Widget::setEvent);

        connect(_node, &Node::statusChanged, this, &P402Widget::updateNodeStatus);

        _modes[NodeProfile402::NoMode]->setNode(_node, axis);
        _modes[NodeProfile402::VL]->setNode(_node, axis);
        _modes[NodeProfile402::TQ]->setNode(_node, axis);
        _modes[NodeProfile402::IP]->setNode(_node, axis);
        _modes[NodeProfile402::PP]->setNode(_node, axis);
        _modes[NodeProfile402::DTY]->setNode(_node, axis);
        _modes[NodeProfile402::CP]->setNode(_node, axis);

        updateNodeStatus();
        updateMode(_axis, _nodeProfile402->actualMode());
        updateState();
    }
}

void P402Widget::updateNodeStatus()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED)
        {
            updateMode(_axis, _nodeProfile402->actualMode());
            _stackedWidget->setEnabled(false);
            _modeGroupBox->setEnabled(true);
            //_stateMachineGroupBox->setEnabled(false);
            _statusWordGroupBox->setEnabled(false);
        }
        else
        {
            toggleStartLogger(false);
        }
    }
}

void P402Widget::updateMode(uint8_t axis, NodeProfile402::OperationMode mode)
{
    if (_axis != axis)
    {
        return;
    }

    if ((mode == NodeProfile402::IP) || (mode == NodeProfile402::VL) || (mode == NodeProfile402::TQ) || (mode == NodeProfile402::PP) || (mode == NodeProfile402::DTY)
        || (mode == NodeProfile402::CP))
    {
        P402ModeWidget *mode402 = dynamic_cast<P402ModeWidget *>(_stackedWidget->currentWidget());
        // reset : Patch because the widget Tarqet torque and velocity are not an IndexSpinbox so not read automaticaly
        mode402->reset();

        _option402Action->setChecked(false);
        _stackedWidget->setCurrentWidget(_modes[mode]);
    }
    else
    {
        _option402Action->setChecked(true);
        _stackedWidget->setCurrentWidget(_modes[NodeProfile402::NoMode]);
    }

    int m = _listModeComboBox.indexOf(mode);
    _modeComboBox->setCurrentIndex(m);
    _modeComboBox->setEnabled(true);
    _modeLabel->clear();
}

void P402Widget::updateState()
{
    NodeProfile402::State402 state = _nodeProfile402->currentState();

    switch (state)
    {
        case NodeProfile402::STATE_NotReadyToSwitchOn:
            _statusWordStateLabel->setText(tr("NotReadyToSwitchOn"));
            setCheckableStateMachine(STATE_NotReadyToSwitchOn);
            break;

        case NodeProfile402::STATE_SwitchOnDisabled:
            _statusWordStateLabel->setText(tr("SwitchOnDisabled"));
            setCheckableStateMachine(STATE_SwitchOnDisabled);
            _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
            _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
            _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
            _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_ReadyToSwitchOn:
            _statusWordStateLabel->setText(tr("ReadyToSwitchOn"));
            setCheckableStateMachine(STATE_ReadyToSwitchOn);
            _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
            _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
            _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_SwitchedOn:
            _statusWordStateLabel->setText(tr("SwitchedOn"));
            setCheckableStateMachine(STATE_SwitchedOn);
            _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
            _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
            _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_OperationEnabled:
            _statusWordStateLabel->setText(tr("OperationEnabled"));
            setCheckableStateMachine(STATE_OperationEnabled);
            _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
            _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
            _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(true);
            _haltPushButton->setEnabled(true);
            _haltPushButton->setCheckable(true);
            break;

        case NodeProfile402::STATE_QuickStopActive:
            _statusWordStateLabel->setText(tr("QuickStopActive"));
            setCheckableStateMachine(STATE_QuickStopActive);
            _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
            _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
            _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
            _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(true);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_FaultReactionActive:
            _statusWordStateLabel->setText(tr("FaultReactionActive"));
            setCheckableStateMachine(STATE_FaultReactionActive);
            _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(false);
            _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
            _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
            _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
            _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_Fault:
            _statusWordStateLabel->setText(tr("Fault"));
            setCheckableStateMachine(STATE_Fault);
            _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
            _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
            _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
            _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;
    }

    update();
}

void P402Widget::setModeIndex(int id)
{
    if (!_node)
    {
        return;
    }

    if (id == -1)
    {
        return;
    }
    _modeComboBox->setEnabled(false);
    _modeLabel->setText("Mode change in progress");
    _nodeProfile402->setMode(_listModeComboBox.at(id));
}

void P402Widget::gotoStateOEClicked()
{
    toggleStartLogger(true);
    _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
}

void P402Widget::toggleStartLogger(bool start)
{
    if (!_node)
    {
        return;
    }

    if (start)
    {
        if (_node->status() != Node::STARTED)
        {
            _node->sendStart();
        }

        _nodeProfile402->start(_logTimerSpinBox->value());

        _stackedWidget->setEnabled(true);
        _modeGroupBox->setEnabled(true);
        //_stateMachineGroupBox->setEnabled(true);
        _statusWordGroupBox->setEnabled(true);
    }
    else
    {
        _nodeProfile402->stop();

        _stackedWidget->setEnabled(false);
        _modeGroupBox->setEnabled(true);
        //_stateMachineGroupBox->setEnabled(false);
        _statusWordGroupBox->setEnabled(false);
    }

    if (_startStopAction->isChecked() != start)
    {
        _startStopAction->blockSignals(true);
        _startStopAction->setChecked(start);
        _startStopAction->blockSignals(false);
    }
}

void P402Widget::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _nodeProfile402->start(ms);
    }
}

void P402Widget::readAllObjects()
{
    if (_node)
    {
        if (_stackedWidget->currentWidget() == _modes[NodeProfile402::NoMode])
        {
            _modes[NodeProfile402::NoMode]->readAllObjects();
        }
        else
        {
            _nodeProfile402->readAllObjects();
        }
    }
}

void P402Widget::setEvent(quint8 event)
{
    _informationLabel->setText(tr("False"));
    _warningLabel->setText(tr("False"));

    QString text;
    if (event & NodeProfile402::VoltageEnabled)
    {
        text = _nodeProfile402->event402Str(NodeProfile402::VoltageEnabled);
    }

    if (event & NodeProfile402::Remote)
    {
        if (!text.isEmpty())
        {
            text.append(",\n");
        }
        text.append(_nodeProfile402->event402Str(NodeProfile402::Remote));
    }
    if (event & NodeProfile402::TargetReached)
    {
        if (!text.isEmpty())
        {
            text.append(",\n");
        }
        text.append(_nodeProfile402->event402Str(NodeProfile402::TargetReached));
    }
    if (event & NodeProfile402::ModeSpecific)
    {
        if (!text.isEmpty())
        {
            text.append(",\n");
        }
        text.append(_nodeProfile402->event402Str(NodeProfile402::ModeSpecific));
    }

    _informationLabel->clear();
    _informationLabel->setText(text);

    text.clear();
    if (event & NodeProfile402::InternalLimitActive)
    {
        text.append(_nodeProfile402->event402Str(NodeProfile402::InternalLimitActive));
    }

    if (event & NodeProfile402::Warning)
    {
        if (!text.isEmpty())
        {
            text.append(",\n");
        }
        text.append(_nodeProfile402->event402Str(NodeProfile402::Warning));
    }
    quint16 mode = static_cast<quint16>(_nodeProfile402->actualMode());
    if (mode == 7)
    {
        if (event & NodeProfile402::FollowingError)
        {
            if (!text.isEmpty())
            {
                text.append(",\n");
            }
            text.append(_nodeProfile402->event402Str(NodeProfile402::FollowingError));
        }
    }
    _warningLabel->clear();
    _warningLabel->setText(text);
}

void P402Widget::updateModeComboBox()
{
    QList<NodeProfile402::OperationMode> modeList = _nodeProfile402->modesSupported();
    _modeComboBox->clear();

    for (int i = 0; i < modeList.size(); i++)
    {
        _listModeComboBox.append(modeList.at(i));
        _modeComboBox->insertItem(i, _nodeProfile402->modeStr(modeList.at(i)));
    }
}

void P402Widget::displayOption402()
{
    if (_stackedWidget->currentWidget() == _modes[NodeProfile402::NoMode])
    {
        updateMode(_axis, _nodeProfile402->actualMode());
    }
    else
    {
        _modes[NodeProfile402::NoMode]->readAllObjects();
        _stackedWidget->setCurrentWidget(_modes[NodeProfile402::NoMode]);
    }
}

void P402Widget::stateMachineClicked(int id)
{
    if (!_node)
    {
        return;
    }
    _nodeProfile402->goToState(static_cast<NodeProfile402::State402>(id));
}

void P402Widget::haltClicked()
{
    _nodeProfile402->toggleHalt();
}

void P402Widget::setCheckableStateMachine(int id)
{
    for (int i = 1; i <= 8; i++)
    {
        _stateMachineGroup->button(i)->setCheckable(false);
    }
    _stateMachineGroup->button(id)->setCheckable(true);
    _stateMachineGroup->button(id)->setChecked(true);
}

void P402Widget::createWidgets()
{
    // Widget P402
    _modes.insert(NodeProfile402::VL, new P402VlWidget());
    _modes.insert(NodeProfile402::IP, new P402IpWidget());
    _modes.insert(NodeProfile402::TQ, new P402TqWidget());
    _modes.insert(NodeProfile402::PP, new P402PpWidget());
    _modes.insert(NodeProfile402::DTY, new P402DtyWidget());
    _modes.insert(NodeProfile402::CP, new P402CpWidget());
    _modes.insert(NodeProfile402::NoMode, new P402OptionWidget());

    // Stacked Widget
    _stackedWidget = new QStackedWidget;
    _stackedWidget->setStyleSheet("QStackedWidget {padding: 0 0 0 3px;}");
    _stackedWidget->addWidget(_modes[NodeProfile402::NoMode]);
    _stackedWidget->addWidget(_modes[NodeProfile402::VL]);
    _stackedWidget->addWidget(_modes[NodeProfile402::IP]);
    _stackedWidget->addWidget(_modes[NodeProfile402::TQ]);
    _stackedWidget->addWidget(_modes[NodeProfile402::PP]);
    _stackedWidget->addWidget(_modes[NodeProfile402::DTY]);
    _stackedWidget->addWidget(_modes[NodeProfile402::CP]);
    _stackedWidget->setMinimumWidth(450);

    // Create interface
    QWidget *controlWidget = new QWidget();
    QLayout *controlLayout = new QVBoxLayout(controlWidget);
    controlLayout->setContentsMargins(0, 0, 4, 0);
    controlLayout->setSpacing(0);

    _modeGroupBox = modeWidgets();
    controlLayout->addWidget(_modeGroupBox);
    _stateMachineGroupBox = stateMachineWidgets();
    controlLayout->addWidget(_stateMachineGroupBox);
    _controlWordGroupBox = controlWordWidgets();
    controlLayout->addWidget(_controlWordGroupBox);
    _statusWordGroupBox = statusWordWidgets();
    controlLayout->addWidget(_statusWordGroupBox);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(controlWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMaximumWidth(370);
    scrollArea->setMinimumWidth(200);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);
    hBoxLayout->addWidget(scrollArea);
    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::VLine);
    frame->setFrameShadow(QFrame::Sunken);
    hBoxLayout->addWidget(frame);
    hBoxLayout->addWidget(_stackedWidget);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(2, 2, 2, 2);
    vBoxLayout->setSpacing(2);
    vBoxLayout->addWidget(toolBarWidgets());
    vBoxLayout->addLayout(hBoxLayout);
    setLayout(vBoxLayout);
}

QToolBar *P402Widget::toolBarWidgets()
{
    QToolBar *toolBar = new QToolBar(tr("Axis commands"));
    toolBar->setIconSize(QSize(20, 20));

    _startStopAction = toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    QIcon iconStartStop;
    iconStartStop.addFile(":/icons/img/icons8-stop.png", QSize(), QIcon::Normal, QIcon::On);
    iconStartStop.addFile(":/icons/img/icons8-play.png", QSize(), QIcon::Normal, QIcon::Off);
    _startStopAction->setIcon(iconStartStop);
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &P402Widget::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setLogTimer(i); });

    _option402Action = new QAction();
    _option402Action->setCheckable(true);
    _option402Action->setIcon(QIcon(":/icons/img/icons8-settings.png"));
    _option402Action->setToolTip(tr("Option code"));
    connect(_option402Action, &QAction::triggered, this, &P402Widget::displayOption402);

    // read all action
    QAction *readAllObjectAction = toolBar->addAction(tr("Read all objects"));
    readAllObjectAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllObjectAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllObjectAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllObjectAction, &QAction::triggered, this, &P402Widget::readAllObjects);

    toolBar->addWidget(_logTimerSpinBox);
    toolBar->addSeparator();
    toolBar->addAction(_option402Action);
    toolBar->addAction(readAllObjectAction);

    return toolBar;
}

QGroupBox *P402Widget::modeWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Mode"));
    QFormLayout *layout = new QFormLayout();

    _modeComboBox = new QComboBox();
    layout->addRow(new QLabel(tr("Modes of operation (0x6n60):")));
    layout->addRow(_modeComboBox);

    _modeLabel = new QLabel();
    layout->addWidget(_modeLabel);
    groupBox->setLayout(layout);

    return groupBox;
}

QGroupBox *P402Widget::stateMachineWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("State Machine"));
    groupBox->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    QFormLayout *layout = new QFormLayout();

    _stateMachineGroup = new QButtonGroup(this);
    _stateMachineGroup->setExclusive(true);

    QPushButton *stateNotReadyToSwitchOnPushButton = new QPushButton(tr("1_Not ready to switch on"));
    stateNotReadyToSwitchOnPushButton->setEnabled(false);
    layout->addRow(stateNotReadyToSwitchOnPushButton);
    _stateMachineGroup->addButton(stateNotReadyToSwitchOnPushButton, STATE_NotReadyToSwitchOn);
    QPushButton *stateSwitchOnDisabledPushButton = new QPushButton(tr("2_Switch on disabled"));
    layout->addRow(stateSwitchOnDisabledPushButton);
    _stateMachineGroup->addButton(stateSwitchOnDisabledPushButton, STATE_SwitchOnDisabled);
    QPushButton *stateReadyToSwitchOnPushButton = new QPushButton(tr("3_Ready to switch on"));
    layout->addRow(stateReadyToSwitchOnPushButton);
    _stateMachineGroup->addButton(stateReadyToSwitchOnPushButton, STATE_ReadyToSwitchOn);
    QPushButton *stateSwitchedOnPushButton = new QPushButton(tr("4_Switched on"));
    layout->addRow(stateSwitchedOnPushButton);
    _stateMachineGroup->addButton(stateSwitchedOnPushButton, STATE_SwitchedOn);
    QPushButton *stateOperationEnabledPushButton = new QPushButton(tr("5_Operation enabled"));
    layout->addRow(stateOperationEnabledPushButton);
    _stateMachineGroup->addButton(stateOperationEnabledPushButton, STATE_OperationEnabled);
    QPushButton *stateQuickStopActivePushButton = new QPushButton(tr("6_Quick stop active"));
    layout->addRow(stateQuickStopActivePushButton);
    _stateMachineGroup->addButton(stateQuickStopActivePushButton, STATE_QuickStopActive);
    QPushButton *stateFaultReactionActivePushButton = new QPushButton(tr("7_Fault reaction active"));
    stateFaultReactionActivePushButton->setEnabled(false);
    layout->addRow(stateFaultReactionActivePushButton);
    _stateMachineGroup->addButton(stateFaultReactionActivePushButton, STATE_FaultReactionActive);
    QPushButton *stateFaultPushButton = new QPushButton(tr("8_Fault"));
    stateFaultPushButton->setEnabled(false);
    layout->addRow(stateFaultPushButton);
    _stateMachineGroup->addButton(stateFaultPushButton, STATE_Fault);

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(_stateMachineGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { stateMachineClicked(id); });
#else
    connect(_stateMachineGroup, QOverload<int>::of(&QButtonGroup::idClicked), [=](int id) { stateMachineClicked(id); });
#endif

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox *P402Widget::controlWordWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Control Word (0x6n40)"));
    QFormLayout *layout = new QFormLayout();

    _haltPushButton = new QPushButton(tr("Halt"));
    _haltPushButton->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    _haltPushButton->setEnabled(false);
    layout->addRow(_haltPushButton);
    groupBox->setLayout(layout);
    connect(_haltPushButton, &QPushButton::clicked, this, &P402Widget::haltClicked);

    _controlWordLabel = new IndexLabel();
    _controlWordLabel->setDisplayHint(AbstractIndexWidget::DisplayHexa);
    layout->addRow(tr("ControlWord sended:"), _controlWordLabel);

    QPushButton *_gotoOEPushButton = new QPushButton(tr("Operation enabled quickly"));
    layout->addRow(_gotoOEPushButton);
    connect(_gotoOEPushButton, &QPushButton::clicked, this, &P402Widget::gotoStateOEClicked);

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox *P402Widget::statusWordWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Status Word (0x6n41)"));
    QFormLayout *layout = new QFormLayout();

    _statusWordLabel = new IndexLabel();
    _statusWordLabel->setDisplayHint(AbstractIndexWidget::DisplayHexa);
    layout->addRow(tr("Raw:"), _statusWordLabel);

    _statusWordStateLabel = new QLabel();
    layout->addRow(tr("State:"), _statusWordStateLabel);

    _informationLabel = new QLabel();
    layout->addRow(tr("Information:"), _informationLabel);

    _warningLabel = new QLabel();
    _warningLabel->setStyleSheet("QLabel { color : red; }");
    layout->addRow(tr("Warning:"), _warningLabel);

    groupBox->setLayout(layout);

    return groupBox;
}
