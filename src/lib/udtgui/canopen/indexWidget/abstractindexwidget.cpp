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

#include "abstractindexwidget.h"

#include "node.h"

#include <QDebug>

AbstractIndexWidget::AbstractIndexWidget(const NodeObjectId &objId)
{
    setObjId(objId);

    _hint = DisplayDirectValue;
    _bitMask = 0xFFFFFFFFFFFFFFFF;
    _offset = 0.0;
    _scale = 1.0;

    _requestRead = false;
}

Node *AbstractIndexWidget::node() const
{
    return nodeInterrest();
}

void AbstractIndexWidget::setNode(Node *node)
{
    setNodeInterrest(node);
    if (node)
    {
        _objId.setBusId(node->busId());
        _objId.setNodeId(node->nodeId());
        if (_objId.isValid())
        {
            _lastValue = nodeInterrest()->nodeOd()->value(_objId);
            setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Normal);
        }
        updateObjId();
    }
    else
    {
        _objId.setBusId(0xFF);
        _objId.setNodeId(0xFF);
    }
}

void AbstractIndexWidget::requestWriteValue(const QVariant &value)
{
    if (!nodeInterrest())
    {
        return;
    }

    switch (inBound(value))
    {
        case AbstractIndexWidget::BoundTooLow:
            _pendingValue = _minValue;
            break;
        case AbstractIndexWidget::BoundOK:
            _pendingValue = value;
            break;
        case AbstractIndexWidget::BoundTooHigh:
            _pendingValue = _maxValue;
            break;
    }

    if (_scale != 0 && _pendingValue.canConvert(QMetaType::Double))
    {
        _pendingValue = _pendingValue.toDouble() / _scale;
    }

    switch (_hint)
    {
        case AbstractIndexWidget::DisplayDirectValue:
        case AbstractIndexWidget::DisplayHexa:
            _pendingValue = static_cast<int32_t>(_pendingValue.toDouble());
            break;

        case AbstractIndexWidget::DisplayQ15_16:
        case AbstractIndexWidget::DisplayQ1_15:
            _pendingValue = static_cast<int32_t>(_pendingValue.toDouble() * 65536.0);
            break;

        case AbstractIndexWidget::DisplayFloat:
            break;
    }

    nodeInterrest()->writeObject(_objId, _pendingValue);
    setDisplayValue(pValue(_pendingValue, _hint), DisplayAttribute::PendingValue);
}

void AbstractIndexWidget::requestReadValue()
{
    if (!nodeInterrest())
    {
        return;
    }
    _requestRead = true;
    nodeInterrest()->readObject(_objId);
}

void AbstractIndexWidget::cancelEdit()
{
    if (!nodeInterrest())
    {
        setDisplayValue(QVariant(), DisplayAttribute::Error);
        return;
    }
    setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Normal);
}

void AbstractIndexWidget::updateHint()
{
}

void AbstractIndexWidget::updateObjId()
{
}

QVariant AbstractIndexWidget::pValue(const QVariant &value, const AbstractIndexWidget::DisplayHint hint) const
{
    QVariant val = value;

    if (_bitMask != 0xFFFFFFFFFFFFFFFF)
    {
        val = value.toLongLong() & _bitMask;
    }

    if (_offset != 0.0)
    {
        val = value.toDouble() + _offset;
    }

    if (_scale != 1.0)
    {
        val = value.toDouble() * _scale;
    }

    if (hint == AbstractIndexWidget::DisplayQ1_15 || hint == AbstractIndexWidget::DisplayQ15_16)
    {
        val = value.toDouble() / 65536.0;
    }

    return val;
}

QString AbstractIndexWidget::pstringValue(const QVariant &value, const AbstractIndexWidget::DisplayHint hint) const
{
    QString str;
    switch (hint)
    {
        case AbstractIndexWidget::DisplayDirectValue:
            if (value.userType() != QMetaType::QString && value.userType() != QMetaType::QByteArray)
            {
                str = QString::number(value.toInt());
            }
            else
            {
                str = value.toString();
            }
            break;

        case AbstractIndexWidget::DisplayHexa:
            str = "0x" + QString::number(value.toInt(), 16).toUpper();
            break;

        case AbstractIndexWidget::DisplayQ1_15:
        case AbstractIndexWidget::DisplayQ15_16:
        case AbstractIndexWidget::DisplayFloat:
            str = QString::number(value.toDouble(), 'g', 6);
            break;
    }

    if (!_unit.isEmpty())
    {
        str.append(_unit);
    }

    return str;
}

AbstractIndexWidget::Bound AbstractIndexWidget::inBound(const QVariant &value)
{
    switch (_hint)
    {
        case AbstractIndexWidget::DisplayDirectValue:
        case AbstractIndexWidget::DisplayHexa:
            if (_maxValue.isValid() && value.toInt() > _maxValue.toInt())
            {
                return BoundTooHigh;
            }
            if (_minValue.isValid() && value.toInt() < _minValue.toInt())
            {
                return BoundTooLow;
            }
            break;

        case AbstractIndexWidget::DisplayQ15_16:
        case AbstractIndexWidget::DisplayQ1_15:
        case AbstractIndexWidget::DisplayFloat:
            if (_maxValue.isValid() && value.toDouble() > _maxValue.toDouble())
            {
                return BoundTooHigh;
            }
            if (_minValue.isValid() && value.toDouble() < _minValue.toDouble())
            {
                return BoundTooLow;
            }
            break;
    }

    return BoundOK;
}

const QVariant &AbstractIndexWidget::minValue() const
{
    return _minValue;
}

void AbstractIndexWidget::setMinValue(const QVariant &minValue)
{
    _minValue = minValue;
}

const QVariant &AbstractIndexWidget::maxValue() const
{
    return _maxValue;
}

void AbstractIndexWidget::setMaxValue(const QVariant &maxValue)
{
    _maxValue = maxValue;
}

void AbstractIndexWidget::setRangeValue(const QVariant &minValue, const QVariant &maxValue)
{
    _minValue = minValue;
    _maxValue = maxValue;
}

QString AbstractIndexWidget::unit() const
{
    return _unit;
}

void AbstractIndexWidget::setUnit(const QString &unit)
{
    _unit = unit;
}

double AbstractIndexWidget::scale() const
{
    return _scale;
}

void AbstractIndexWidget::setScale(double scale)
{
    if (scale == 0.0)
    {
        scale = 1.0;
    }
    _scale = scale;
}

double AbstractIndexWidget::offset() const
{
    return _offset;
}

void AbstractIndexWidget::setOffset(double offset)
{
    _offset = offset;
}

uint64_t AbstractIndexWidget::bitMask() const
{
    return _bitMask;
}

void AbstractIndexWidget::setBitMask(const uint64_t &bitMask)
{
    _bitMask = bitMask;
}

AbstractIndexWidget::DisplayHint AbstractIndexWidget::hint() const
{
    return _hint;
}

void AbstractIndexWidget::setDisplayHint(const AbstractIndexWidget::DisplayHint hint)
{
    if (_hint != hint)
    {
        _hint = hint;
        updateHint();
        if (!nodeInterrest())
        {
            return;
        }
        setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Normal);
    }
}

QVariant AbstractIndexWidget::value() const
{
    return pValue(_lastValue, _hint);
}

QString AbstractIndexWidget::stringValue() const
{
    return pstringValue(value(), _hint);
}

void AbstractIndexWidget::readObject()
{
    if (!nodeInterrest())
    {
        return;
    }
    nodeInterrest()->readObject(_objId);
}

const NodeObjectId &AbstractIndexWidget::objId() const
{
    return _objId;
}

void AbstractIndexWidget::setObjId(const NodeObjectId &objId)
{
    if (_objId.isValid())
    {
        unRegisterObjId(_objId);
    }
    _objId = objId;

    if (objId.isASubIndex())
    {
        registerObjId(_objId);
    }
    Node *node = _objId.node();
    if (node)
    {
        setNode(node);
        return;
    }
    updateObjId();
}

void AbstractIndexWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (!nodeInterrest())
    {
        return;
    }

    _lastValue = nodeInterrest()->nodeOd()->value(objId);
    if (flags & SDO::Error)
    {
        if (_pendingValue.isValid() && (flags & SDO::Write)) // we request a write value that cause an error
        {
            setDisplayValue(pValue(_pendingValue, _hint), DisplayAttribute::Error);
            _pendingValue = QVariant();
            return;
        }
        else if (flags & SDO::Read) // any read cause an error
        {
            if (isEditing() && !_requestRead)
            {
                return;
            }
            setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Error);
            _requestRead = false;
            return;
        }
        else // any other write request failed
        {
            return;
        }
    }
    if (flags & SDO::Read && this->isEditing() && !_requestRead)
    {
        return;
    }
    setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Normal);
    _requestRead = false;
    _pendingValue = QVariant();
}
