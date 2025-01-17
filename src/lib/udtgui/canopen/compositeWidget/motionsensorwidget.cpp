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

#include "motionsensorwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexcombobox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "indexdb402.h"
#include "node.h"
#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSplitter>
#include <QWidget>

MotionSensorWidget::MotionSensorWidget(QWidget *parent) : QWidget(parent)
{
    _nodeProfile402 = nullptr;
    createWidgets();
    _state = NONE;
    _mode = MODE_SENSOR_NONE;
}

Node *MotionSensorWidget::node() const
{
    return _node;
}

QString MotionSensorWidget::title() const
{
    switch (_mode)
    {
        case MotionSensorWidget::MODE_SENSOR_NONE:
            return tr("None");
        case MODE_SENSOR_POSITION:
            return tr("Position sensor");
        case MODE_SENSOR_VELOCITY:
            return tr("Velocity sensor");
        case MODE_SENSOR_TORQUE:
            return tr("Torque sensor");
    }
    return QString();
}

void MotionSensorWidget::setNode(Node *node, uint8_t axis)
{
    _node = node;
    if (!_node)
    {
        return;
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    _dataLogger->removeAllData();

    connect(_node, &Node::statusChanged, this, &MotionSensorWidget::statusNodeChanged);
    if (!_node->profiles().isEmpty())
    {
        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
        connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &MotionSensorWidget::stateChanged);
    }
    setIMode();

    _sensorSelectComboBox->setNode(node);
    _frequencyDividerSpinBox->setNode(node);
    _configBitCheckBox->setNode(node);
    _sensorParam0SpinBox->setNode(node);
    _sensorParam1SpinBox->setNode(node);
    _sensorParam2SpinBox->setNode(node);
    _sensorParam3SpinBox->setNode(node);

    _thresholdMinSpinBox->setNode(node);
    _thresholdMaxSpinBox->setNode(node);
    _thresholdModeComboBox->setNode(node);
    _preOffsetSpinBox->setNode(node);
    _scaleSpinBox->setNode(node);
    _postOffsetSpinBox->setNode(node);
    _errorMinSpinBox->setNode(node);
    _errorMaxSpinBox->setNode(node);

    _rawDataValueLabel->setNode(node);
    _flagLabel->setNode(node);
    _valueLabel->setNode(node);

    _filterSelect->setNode(node);
    _filterParam0SpinBox->setNode(node);
    _filterParam1SpinBox->setNode(node);
    _filterParam2SpinBox->setNode(node);
    _filterParam3SpinBox->setNode(node);
}

void MotionSensorWidget::setMode(MotionSensorWidget::ModeSensor mode)
{
    _mode = mode;
    setIMode();
}

void MotionSensorWidget::setLogTimer(int ms)
{
    if (_dataLogger->isStarted())
    {
        _dataLogger->start(ms);
    }
}

void MotionSensorWidget::setIMode()
{
    NodeObjectId rawDataValueLabel_ObjId;
    NodeObjectId flagLabel_ObjId;
    NodeObjectId valueLabel_ObjId;

    if (!_nodeProfile402)
    {
        return;
    }

    IndexDb402::OdMode402 odMode402 = IndexDb402::MODE402_TORQUE;

    switch (_mode)
    {
        case MODE_SENSOR_NONE:
            break;

        case MODE_SENSOR_TORQUE:
            _sensorConfigGroupBox->setTitle(tr("Torque sensor configuration"));
            _filterGroupBox->setTitle(tr("Torque filter"));
            _conditioningGroupBox->setTitle(tr("Torque conditioning"));
            _statusGroupBox->setTitle(tr("Torque sensor status"));
            _dataLoggerWidget->setTitle(tr("Node %1 axis %2 torque motion sensor").arg(_node->nodeId()).arg(_axis));
            odMode402 = IndexDb402::MODE402_TORQUE;
            break;

        case MODE_SENSOR_VELOCITY:
            _sensorConfigGroupBox->setTitle(tr("Velocity sensor configuration"));
            _filterGroupBox->setTitle(tr("Velocity filter"));
            _conditioningGroupBox->setTitle(tr("Velocity conditioning"));
            _statusGroupBox->setTitle(tr("Velocity sensor status"));
            _dataLoggerWidget->setTitle(tr("Node %1 axis %2 velocity motion sensor").arg(_node->nodeId()).arg(_axis));
            odMode402 = IndexDb402::MODE402_VELOCITY;
            break;

        case MODE_SENSOR_POSITION:
            _sensorConfigGroupBox->setTitle(tr("Position sensor configuration"));
            _filterGroupBox->setTitle(tr("Position filter"));
            _conditioningGroupBox->setTitle(tr("Position conditioning"));
            _statusGroupBox->setTitle(tr("Position sensor status"));
            _dataLoggerWidget->setTitle(tr("Node %1 axis %2 position motion sensor").arg(_node->nodeId()).arg(_axis));
            odMode402 = IndexDb402::MODE402_POSITION;
            break;
    }

    // Config
    _sensorSelectComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_SELECT, _axis, odMode402));
    _frequencyDividerSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FREQUENCY_DIVIDER, _axis, odMode402));
    _configBitCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_CONFIG_BIT, _axis, odMode402));
    _sensorParam0SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PARAM_0, _axis, odMode402));
    _sensorParam1SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PARAM_1, _axis, odMode402));
    _sensorParam2SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PARAM_2, _axis, odMode402));
    _sensorParam3SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PARAM_3, _axis, odMode402));

    // Filter
    _filterSelect->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_SELECT, _axis, odMode402));
    _filterParam0SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_PARAM_0, _axis, odMode402));
    _filterParam1SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_PARAM_1, _axis, odMode402));
    _filterParam2SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_PARAM_2, _axis, odMode402));
    _filterParam3SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_PARAM_3, _axis, odMode402));

    // Conditioning
    _thresholdMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_THRESHOLD_MIN, _axis, odMode402));
    _thresholdMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_THRESHOLD_MAX, _axis, odMode402));
    _thresholdModeComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_THRESHOLD_MODE, _axis, odMode402));
    _preOffsetSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PRE_OFFSET, _axis, odMode402));
    _scaleSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_SCALE, _axis, odMode402));
    _postOffsetSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_POST_OFFSET, _axis, odMode402));
    _errorMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_ERROR_MIN, _axis, odMode402));
    _errorMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_ERROR_MAX, _axis, odMode402));

    rawDataValueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_SENSOR_STATUS_RAWDATA, _axis, odMode402);
    flagLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_SENSOR_STATUS_FLAG, _axis, odMode402);
    valueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_SENSOR_STATUS_VALUE, _axis, odMode402);

    // Status
    _rawDataValueLabel->setObjId(rawDataValueLabel_ObjId);
    _flagLabel->setObjId(flagLabel_ObjId);
    _valueLabel->setObjId(valueLabel_ObjId);
    rawDataValueLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    flagLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    valueLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());

    // Datalogger
    _dataLogger->removeAllData();
    _dataLogger->addData(rawDataValueLabel_ObjId);
    _dataLogger->addData(valueLabel_ObjId);
}

void MotionSensorWidget::createWidgets()
{
    _dataLogger = new DataLogger();

    QWidget *motionSensorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(motionSensorWidget);
    actionLayout->setContentsMargins(0, 0, 4, 0);
    actionLayout->setSpacing(0);

    actionLayout->addWidget(createInformationWidgets());
    _sensorConfigGroupBox = createSensorConfigurationWidgets();
    actionLayout->addWidget(_sensorConfigGroupBox);
    _filterGroupBox = createSensorFilterWidgets();
    actionLayout->addWidget(_filterGroupBox);
    _conditioningGroupBox = createSensorConditioningWidgets();
    actionLayout->addWidget(_conditioningGroupBox);
    _statusGroupBox = createSensorStatusWidgets();
    actionLayout->addWidget(_statusGroupBox);

    QScrollArea *motionSensorScrollArea = new QScrollArea;
    motionSensorScrollArea->setWidget(motionSensorWidget);
    motionSensorScrollArea->setWidgetResizable(true);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setStyleSheet("QSplitter {background: #19232D;}");
    splitter->addWidget(motionSensorScrollArea);

    QWidget *widgetLogger = new QWidget();
    QVBoxLayout *layoutLogger = new QVBoxLayout();
    layoutLogger->setContentsMargins(4, 4, 0, 0);
    _dataLoggerWidget = new DataLoggerWidget(_dataLogger, Qt::Vertical);
    _dataLoggerWidget->setType(DataLoggerWidget::InternalType);
    layoutLogger->addWidget(_dataLoggerWidget);
    widgetLogger->setLayout(layoutLogger);
    splitter->addWidget(widgetLogger);
    splitter->setSizes(QList<int>() << 100 << 300);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(2, 2, 2, 2);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(createToolBarWidgets());
    vBoxLayout->addWidget(splitter);
    setLayout(vBoxLayout);
}

QToolBar *MotionSensorWidget::createToolBarWidgets()
{
    // toolbar
    QToolBar *toolBar = new QToolBar(tr("Data logger commands"));
    toolBar->setIconSize(QSize(20, 20));

    // start stop
    toolBar->addAction(_dataLoggerWidget->managerWidget()->startStopAction());

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setStatusTip(tr("Sets the interval of log timer in ms"));
    toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setLogTimer(i); });

    // clear
    QAction *action;
    action = toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    action->setStatusTip(tr("Clear all data"));
    connect(action, &QAction::triggered, _dataLogger, &DataLogger::clear);

    toolBar->addSeparator();

    // read all action
    QAction *readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &MotionSensorWidget::readAllObject);

    return toolBar;
}

QGroupBox *MotionSensorWidget::createInformationWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Information"));
    groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QVBoxLayout *vLayout = new QVBoxLayout();

    _informationLabel = new QLabel();
    _informationLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    vLayout->addWidget(_informationLabel);

    _enableButton = new QPushButton(tr("Unlock config, Go to \"Switched On\""));
    vLayout->addWidget(_enableButton);
    connect(_enableButton, &QPushButton::clicked, this, &MotionSensorWidget::goEnableButton);

    groupBox->setLayout(vLayout);
    return groupBox;
}

QGroupBox *MotionSensorWidget::createSensorConfigurationWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sensor config"));
    QFormLayout *formLayout = new QFormLayout();

    _sensorSelectComboBox = new IndexComboBox();
    _sensorSelectComboBox->addItem(tr("OFF"), QVariant(static_cast<uint16_t>(0x0000)));

    _sensorSelectComboBox->addItem(tr("TORQUE_FROM_MOTOR"), QVariant(static_cast<uint16_t>(0x1100)));
    _sensorSelectComboBox->addItem(tr("VELOCITY_FROM_MOTOR"), QVariant(static_cast<uint16_t>(0x1200)));
    _sensorSelectComboBox->addItem(tr("POSITION_FROM_MOTOR"), QVariant(static_cast<uint16_t>(0x1300)));

    _sensorSelectComboBox->addItem(tr("POSITION_FROM_VELOCITY"), QVariant(static_cast<uint16_t>(0x2200)));
    _sensorSelectComboBox->addItem(tr("VELOCITY_FROM_POSITION"), QVariant(static_cast<uint16_t>(0x2300)));

    _sensorSelectComboBox->addItem(tr("QEI_CH1"), QVariant(static_cast<uint16_t>(0x3101)));
    _sensorSelectComboBox->addItem(tr("QEI_CH2"), QVariant(static_cast<uint16_t>(0x3102)));

    _sensorSelectComboBox->addItem(tr("SSI_CH1"), QVariant(static_cast<uint16_t>(0x3201)));
    _sensorSelectComboBox->addItem(tr("SSI_CH2"), QVariant(static_cast<uint16_t>(0x3202)));

    _sensorSelectComboBox->addItem(tr("ANALOG_CH1"), QVariant(static_cast<uint16_t>(0x4001)));
    _sensorSelectComboBox->addItem(tr("ANALOG_CH2"), QVariant(static_cast<uint16_t>(0x4002)));
    formLayout->addRow(tr("&Sensor select:"), _sensorSelectComboBox);

    _frequencyDividerSpinBox = new IndexSpinBox();
    _frequencyDividerSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    _frequencyDividerSpinBox->setRangeValue(1, 1000);
    formLayout->addRow(tr("&Frequency divider:"), _frequencyDividerSpinBox);

    _configBitCheckBox = new IndexCheckBox();
    _configBitCheckBox->setBitMask(32);
    formLayout->addRow(tr("Q15.16:"), _configBitCheckBox);

    _sensorParam0SpinBox = new IndexSpinBox();
    formLayout->addRow(tr("Param 0:"), _sensorParam0SpinBox);

    _sensorParam1SpinBox = new IndexSpinBox();
    formLayout->addRow(tr("Param 1:"), _sensorParam1SpinBox);

    _sensorParam2SpinBox = new IndexSpinBox();
    formLayout->addRow(tr("Param 2:"), _sensorParam2SpinBox);

    _sensorParam3SpinBox = new IndexSpinBox();
    formLayout->addRow(tr("Param 3:"), _sensorParam3SpinBox);

    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *MotionSensorWidget::createSensorFilterWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sensor filter"));
    QFormLayout *formLayout = new QFormLayout();

    _filterSelect = new IndexComboBox();
    _filterSelect->addItem(tr("OFF"), QVariant(static_cast<uint16_t>(0x0000)));
    _filterSelect->addItem(tr("LPF"), QVariant(static_cast<uint16_t>(0x1000)));
    _filterSelect->addItem(tr("AVERAGING"), QVariant(static_cast<uint16_t>(0x2000)));
    _filterSelect->addItem(tr("INTEGRATOR"), QVariant(static_cast<uint16_t>(0x8000)));
    formLayout->addRow(tr("Filter select:"), _filterSelect);

    _filterParam0SpinBox = new IndexSpinBox();
    _filterParam0SpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Param 0:"), _filterParam0SpinBox);

    _filterParam1SpinBox = new IndexSpinBox();
    _filterParam1SpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Param 1:"), _filterParam1SpinBox);

    _filterParam2SpinBox = new IndexSpinBox();
    _filterParam2SpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Param 2:"), _filterParam2SpinBox);

    _filterParam3SpinBox = new IndexSpinBox();
    _filterParam3SpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Param 3:"), _filterParam3SpinBox);

    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *MotionSensorWidget::createSensorConditioningWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sensor conditioning"));
    QFormLayout *formLayout = new QFormLayout();

    _preOffsetSpinBox = new IndexSpinBox();
    _preOffsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Pr&e offset:"), _preOffsetSpinBox);

    _scaleSpinBox = new IndexSpinBox();
    _scaleSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("S&cale:"), _scaleSpinBox);

    _postOffsetSpinBox = new IndexSpinBox();
    _postOffsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&Post offset:"), _postOffsetSpinBox);

    QLayout *errorRangelayout = new QHBoxLayout();
    errorRangelayout->setSpacing(0);
    _errorMinSpinBox = new IndexSpinBox();
    _errorMinSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    errorRangelayout->addWidget(_errorMinSpinBox);
    QLabel *errorRangeSepLabel = new QLabel(tr("-"));
    errorRangeSepLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    errorRangelayout->addWidget(errorRangeSepLabel);
    _errorMaxSpinBox = new IndexSpinBox();
    _errorMaxSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    errorRangelayout->addWidget(_errorMaxSpinBox);
    QLabel *errorRangeLabel = new QLabel(tr("&Error range:"));
    errorRangeLabel->setBuddy(_errorMinSpinBox);
    formLayout->addRow(errorRangeLabel, errorRangelayout);

    _thresholdModeComboBox = new IndexComboBox();
    _thresholdModeComboBox->addItem(tr("No threshold"), QVariant(static_cast<uint16_t>(0x0000)));
    _thresholdModeComboBox->addItem(tr("Min-max"), QVariant(static_cast<uint16_t>(0x0001)));
    _thresholdModeComboBox->addItem(tr("Modulo"), QVariant(static_cast<uint16_t>(0x0002)));
    _thresholdModeComboBox->addItem(tr("Old value"), QVariant(static_cast<uint16_t>(0x0003)));
    formLayout->addRow(tr("Th&reshold mode:"), _thresholdModeComboBox);

    QLayout *thresholdlayout = new QHBoxLayout();
    thresholdlayout->setSpacing(0);
    _thresholdMinSpinBox = new IndexSpinBox();
    _thresholdMinSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    thresholdlayout->addWidget(_thresholdMinSpinBox);
    QLabel *thresholdSepLabel = new QLabel(tr("-"));
    thresholdSepLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    thresholdlayout->addWidget(thresholdSepLabel);
    _thresholdMaxSpinBox = new IndexSpinBox();
    _thresholdMaxSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    thresholdlayout->addWidget(_thresholdMaxSpinBox);
    QLabel *thresholdLabel = new QLabel(tr("&Threshold:"));
    thresholdLabel->setBuddy(_thresholdMinSpinBox);
    formLayout->addRow(thresholdLabel, thresholdlayout);

    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *MotionSensorWidget::createSensorStatusWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sensor status"));
    QFormLayout *formLayout = new QFormLayout();

    _rawDataValueLabel = new IndexLabel();
    _rawDataValueLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Ra&w Data:"), _rawDataValueLabel);

    _flagLabel = new IndexLabel();
    formLayout->addRow(tr("&Flag:"), _flagLabel);

    _valueLabel = new IndexLabel();
    _valueLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&Value:"), _valueLabel);

    groupBox->setLayout(formLayout);

    return groupBox;
}

void MotionSensorWidget::statusNodeChanged(Node::Status status)
{
    if (status == Node::STOPPED)
    {
        this->setEnabled(false);
    }
    else
    {
        this->setEnabled(true);
    }
}

void MotionSensorWidget::stateChanged()
{
    if (_nodeProfile402->currentState() == NodeProfile402::STATE_OperationEnabled)
    {
        _sensorConfigGroupBox->setEnabled(false);
        _filterGroupBox->setEnabled(false);
        _conditioningGroupBox->setEnabled(false);
        _enableButton->setEnabled(true);
        _informationLabel->setText(tr("Not available in \"Operation Enabled\""));
    }
    else
    {
        _sensorConfigGroupBox->setEnabled(true);
        _filterGroupBox->setEnabled(true);
        _conditioningGroupBox->setEnabled(true);
        _enableButton->setEnabled(false);
        _informationLabel->setText("");
    }
}

void MotionSensorWidget::readAllObject()
{
    _sensorSelectComboBox->readObject();
    _frequencyDividerSpinBox->readObject();
    _configBitCheckBox->readObject();
    _sensorParam0SpinBox->readObject();
    _sensorParam1SpinBox->readObject();
    _sensorParam2SpinBox->readObject();
    _sensorParam3SpinBox->readObject();

    _thresholdMinSpinBox->readObject();
    _thresholdMaxSpinBox->readObject();
    _thresholdModeComboBox->readObject();
    _preOffsetSpinBox->readObject();
    _scaleSpinBox->readObject();
    _postOffsetSpinBox->readObject();
    _errorMinSpinBox->readObject();
    _errorMaxSpinBox->readObject();

    _rawDataValueLabel->readObject();
    _flagLabel->readObject();
    _valueLabel->readObject();

    _filterSelect->readObject();
    _filterParam0SpinBox->readObject();
    _filterParam1SpinBox->readObject();
    _filterParam2SpinBox->readObject();
    _filterParam3SpinBox->readObject();
}

void MotionSensorWidget::goEnableButton()
{
    _nodeProfile402->goToState(NodeProfile402::STATE_SwitchedOn);
}
