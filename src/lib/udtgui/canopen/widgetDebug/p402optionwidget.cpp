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

#include "p402optionwidget.h"

#include "indexdb402.h"
#include "node.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>

P402OptionWidget::P402OptionWidget(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
}

P402OptionWidget::~P402OptionWidget()
{
    unRegisterFullOd();
}

Node *P402OptionWidget::node() const
{
    return _node;
}

void P402OptionWidget::setNode(Node *node, uint8_t axis)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402OptionWidget::updateData);
        }
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    _node = node;
    if (_node)
    {
        _abortConnectionObjectId = IndexDb402::getObjectId(IndexDb402::OD_ABORT_CONNECTION_OPTION, axis);
        _quickStopObjectId = IndexDb402::getObjectId(IndexDb402::OD_QUICK_STOP_OPTION, axis);
        _shutdownObjectId = IndexDb402::getObjectId(IndexDb402::OD_SHUTDOWN_OPTION, axis);
        _disableObjectId = IndexDb402::getObjectId(IndexDb402::OD_DISABLE_OPERATION_OPTION, axis);
        _haltObjectId = IndexDb402::getObjectId(IndexDb402::OD_HALT_OPTION, axis);
        _faultReactionObjectId = IndexDb402::getObjectId(IndexDb402::OD_FAULT_REACTION_OPTION, axis);

        createWidgets();

        registerObjId({_abortConnectionObjectId});
        registerObjId({_quickStopObjectId});
        registerObjId({_shutdownObjectId});
        registerObjId({_disableObjectId});
        registerObjId({_haltObjectId});
        registerObjId({_faultReactionObjectId});

        setNodeInterrest(node);
    }
}

void P402OptionWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED)
        {
            this->setEnabled(true);
            _node->readObject(_abortConnectionObjectId);
            _node->readObject(_quickStopObjectId);
            _node->readObject(_shutdownObjectId);
            _node->readObject(_disableObjectId);
            _node->readObject(_haltObjectId);
            _node->readObject(_faultReactionObjectId);
        }
    }
}

void P402OptionWidget::abortConnectionOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_abortConnectionObjectId, QVariant(value));
}

void P402OptionWidget::quickStopOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_quickStopObjectId, QVariant(value));
}

void P402OptionWidget::shutdownOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_shutdownObjectId, QVariant(value));
}

void P402OptionWidget::disableOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_disableObjectId, QVariant(value));
}

void P402OptionWidget::haltOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_haltObjectId, QVariant(value));
}

void P402OptionWidget::faultReactionOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_faultReactionObjectId, QVariant(value));
}

void P402OptionWidget::refreshData(NodeObjectId object)
{
    if (_node->nodeOd()->indexExist(object.index()))
    {
        int value = _node->nodeOd()->value(object).toInt();

        if (object == _abortConnectionObjectId)
        {
            _abortConnectionOptionGroup->button(value)->setChecked(true);
        }

        if (object == _quickStopObjectId)
        {
            _quickStopOptionGroup->button(value)->setChecked(true);
        }

        if (object == _shutdownObjectId)
        {
            _shutdownOptionGroup->button(value)->setChecked(true);
        }

        if (object == _disableObjectId)
        {
            _disableOptionGroup->button(value)->setChecked(true);
        }

        if (object == _haltObjectId)
        {
            _haltOptionGroup->button(value)->setChecked(true);
        }

        if (object == _faultReactionObjectId)
        {
            _faultReactionOptionGroup->button(value)->setChecked(true);
        }
    }
}

void P402OptionWidget::createWidgets()
{
    QString name;
    // SECOND COLUMM
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box Abort connection option
    name = tr("Abort connection option ") + QString("(0x%1) :").arg(QString::number(_abortConnectionObjectId.index(), 16).toUpper());
    QGroupBox *abortConnectionOptionGroupBox = new QGroupBox(name);
    QFormLayout *abortConnectionOptionLayout = new QFormLayout();
    _abortConnectionOptionGroup = new QButtonGroup(this);
    _abortConnectionOptionGroup->setExclusive(true);
    QRadioButton *_0AbortConnectionOptionCheckBox = new QRadioButton(tr("0 No action"));
    abortConnectionOptionLayout->addRow(_0AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_0AbortConnectionOptionCheckBox, 0);
    QRadioButton *_1AbortConnectionOptionCheckBox = new QRadioButton(tr("1 Fault signal"));
    abortConnectionOptionLayout->addRow(_1AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_1AbortConnectionOptionCheckBox, 1);
    QRadioButton *_2AbortConnectionOptionCheckBox = new QRadioButton(tr("2 Disable voltage command"));
    abortConnectionOptionLayout->addRow(_2AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_2AbortConnectionOptionCheckBox, 2);
    QRadioButton *_3AbortConnectionOptionCheckBox = new QRadioButton(tr("3 Quick stop command"));
    abortConnectionOptionLayout->addRow(_3AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_3AbortConnectionOptionCheckBox, 3);
    abortConnectionOptionGroupBox->setLayout(abortConnectionOptionLayout);
    connect(_abortConnectionOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { abortConnectionOptionClicked(id); });
    // END Group Box Abort connection option

    // Group Box Quick stop option
    name = tr("Quick stop option ") + QString("(0x%1) :").arg(QString::number(_quickStopObjectId.index(), 16).toUpper());
    QGroupBox *quickStopOptionGroupBox = new QGroupBox(name);
    QFormLayout *quickStopOptionLayout = new QFormLayout();
    _quickStopOptionGroup = new QButtonGroup(this);
    _quickStopOptionGroup->setExclusive(true);
    QRadioButton *_0QuickStopOptionCheckBox = new QRadioButton(tr("0 Disable drive function"));
    quickStopOptionLayout->addRow(_0QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_0QuickStopOptionCheckBox, 0);
    QRadioButton *_1QuickStopOptionCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp and transit into switch on disabled"));
    quickStopOptionLayout->addRow(_1QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_1QuickStopOptionCheckBox, 1);
    QRadioButton *_2QuickStopOptionCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp and transit into switch on disabled"));
    quickStopOptionLayout->addRow(_2QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_2QuickStopOptionCheckBox, 2);
    QRadioButton *_5QuickStopOptionCheckBox = new QRadioButton(tr("5 Slow down on slow down ramp and stay in quick stop active"));
    quickStopOptionLayout->addRow(_5QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_5QuickStopOptionCheckBox, 5);
    QRadioButton *_6QuickStopOptionCheckBox = new QRadioButton(tr("6 Slow down on quick stop ramp and stay in quick stop active"));
    quickStopOptionLayout->addRow(_6QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_6QuickStopOptionCheckBox, 6);
    quickStopOptionGroupBox->setLayout(quickStopOptionLayout);
    connect(_quickStopOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { quickStopOptionClicked(id); });
    // END Group Box Quick stop option

    // Group Box Shutdown option code
    name = tr("Shutdown option code ") + QString("(0x%1) :").arg(QString::number(_shutdownObjectId.index(), 16).toUpper());
    QGroupBox *shutdownOptionGroupBox = new QGroupBox(name);
    QFormLayout *shutdownOptionLayout = new QFormLayout();
    _shutdownOptionGroup = new QButtonGroup(this);
    _shutdownOptionGroup->setExclusive(true);
    QRadioButton *_0ShutdownOptionCheckBox = new QRadioButton(tr("0 Disable drive function (switch-off the drive power stage)"));
    shutdownOptionLayout->addRow(_0ShutdownOptionCheckBox);
    _shutdownOptionGroup->addButton(_0ShutdownOptionCheckBox, 0);
    QRadioButton *_1ShutdownOptionCheckBox = new QRadioButton(tr("1 Slow down with slow down ramp; disable of the drive function"));
    shutdownOptionLayout->addRow(_1ShutdownOptionCheckBox);
    _shutdownOptionGroup->addButton(_1ShutdownOptionCheckBox, 1);
    shutdownOptionGroupBox->setLayout(shutdownOptionLayout);
    connect(_shutdownOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { shutdownOptionClicked(id); });
    // END Group Box Shutdown option code

    // Group Box Disable operation option code
    name = tr("Disable operation option code ") + QString("(0x%1) :").arg(QString::number(_disableObjectId.index(), 16).toUpper());
    QGroupBox *disableOptionGroupBox = new QGroupBox(name);
    QFormLayout *disableOptionLayout = new QFormLayout();
    _disableOptionGroup = new QButtonGroup(this);
    _disableOptionGroup->setExclusive(true);
    QRadioButton *_0DisableOptionCheckBox = new QRadioButton(tr("0 Disable drive function (switch-off the drive power stage)"));
    disableOptionLayout->addRow(_0DisableOptionCheckBox);
    _disableOptionGroup->addButton(_0DisableOptionCheckBox, 0);
    QRadioButton *_1DisableOptionCheckBox = new QRadioButton(tr("1 Slow down with slow down ramp; disable of the drive function"));
    disableOptionLayout->addRow(_1DisableOptionCheckBox);
    _disableOptionGroup->addButton(_1DisableOptionCheckBox, 1);
    disableOptionGroupBox->setLayout(disableOptionLayout);
    connect(_disableOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { disableOptionClicked(id); });
    // END Group Box Disable operation option code

    // Group Box Halt option
    name = tr("Halt option ") + QString("(0x%1) :").arg(QString::number(_haltObjectId.index(), 16).toUpper());
    QGroupBox *haltOptionGroupBox = new QGroupBox(name);
    QFormLayout *haltOptionLayout = new QFormLayout();
    _haltOptionGroup = new QButtonGroup(this);
    _haltOptionGroup->setExclusive(true);
    QRadioButton *_slowDownRampCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp and stay in operation enabled"));
    haltOptionLayout->addRow(_slowDownRampCheckBox);
    _haltOptionGroup->addButton(_slowDownRampCheckBox, 1);
    QRadioButton *_quickStopRampCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp and stay in operation enabled"));
    haltOptionLayout->addRow(_quickStopRampCheckBox);
    _haltOptionGroup->addButton(_quickStopRampCheckBox, 2);
    haltOptionGroupBox->setLayout(haltOptionLayout);
    connect(_haltOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { haltOptionClicked(id); });
    // END Group Box Halt option

    // Group Box Fault reaction option
    name = tr("Fault reaction option ") + QString("(0x%1) :").arg(QString::number(_faultReactionObjectId.index(), 16).toUpper());
    QGroupBox *faultReactionOptionGroupBox = new QGroupBox(name);
    QFormLayout *faultReactionOptionLayout = new QFormLayout();
    _faultReactionOptionGroup = new QButtonGroup(this);
    _faultReactionOptionGroup->setExclusive(true);
    QRadioButton *_0FaultReactionOptionCheckBox = new QRadioButton(tr("0 Disable drive function, motor is free to rotate"));
    faultReactionOptionLayout->addRow(_0FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_0FaultReactionOptionCheckBox, 0);
    QRadioButton *_1FaultReactionOptionCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp"));
    faultReactionOptionLayout->addRow(_1FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_1FaultReactionOptionCheckBox, 1);
    QRadioButton *_2FaultReactionOptionCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp"));
    faultReactionOptionLayout->addRow(_2FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_2FaultReactionOptionCheckBox, 2);
    faultReactionOptionGroupBox->setLayout(faultReactionOptionLayout);
    connect(_faultReactionOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { faultReactionOptionClicked(id); });
    // END Group Box Quick stop option

    layout->addWidget(abortConnectionOptionGroupBox);
    layout->addWidget(quickStopOptionGroupBox);
    layout->addWidget(shutdownOptionGroupBox);
    layout->addWidget(disableOptionGroupBox);
    layout->addWidget(haltOptionGroupBox);
    layout->addWidget(faultReactionOptionGroupBox);
    // END SECOND COLUMM

    setLayout(layout);
}

void P402OptionWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if ((objId == _abortConnectionObjectId) || (objId == _quickStopObjectId) || (objId == _shutdownObjectId) || (objId == _disableObjectId) || (objId == _haltObjectId) ||
        (objId == _faultReactionObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        refreshData(objId);
    }
}
