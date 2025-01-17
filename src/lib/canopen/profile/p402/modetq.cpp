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

#include "modetq.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

ModeTq::ModeTq(NodeProfile402 *nodeProfile402) : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_TARGET, _nodeProfile402->axisId());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _torqueDemandObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND, _nodeProfile402->axisId());
    _torqueDemandObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _torqueActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, _nodeProfile402->axisId());
    _torqueActualValueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _targetSlopeObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_SLOPE, _nodeProfile402->axisId());
    _targetSlopeObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _maxTorqueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_TORQUE, _nodeProfile402->axisId());
    _maxTorqueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _mode = NodeProfile402::OperationMode::TQ;

    _cmdControlWordFlag = 0;
}

const NodeObjectId &ModeTq::targetObjectId() const
{
    return _targetObjectId;
}

const NodeObjectId &ModeTq::torqueDemandObjectId() const
{
    return _torqueDemandObjectId;
}

const NodeObjectId &ModeTq::torqueActualValueObjectId() const
{
    return _torqueActualValueObjectId;
}

const NodeObjectId &ModeTq::targetSlopeObjectId() const
{
    return _targetSlopeObjectId;
}

const NodeObjectId &ModeTq::maxTorqueObjectId() const
{
    return _maxTorqueObjectId;
}

void ModeTq::setTarget(qint32 target)
{
    _nodeProfile402->node()->writeObject(_targetObjectId, QVariant(static_cast<qint16>(target)));
}

quint16 ModeTq::getSpecificCwFlag()
{
    return _cmdControlWordFlag;
}

void ModeTq::setCwDefaultflag()
{
    _cmdControlWordFlag = 0;
}

void ModeTq::readRealTimeObjects()
{
    _nodeProfile402->node()->readObject(_torqueDemandObjectId);
    _nodeProfile402->node()->readObject(_torqueActualValueObjectId);
}

void ModeTq::readAllObjects()
{
    _nodeProfile402->node()->readObject(_torqueDemandObjectId);
    _nodeProfile402->node()->readObject(_torqueActualValueObjectId);
    _nodeProfile402->node()->readObject(_targetSlopeObjectId);
    _nodeProfile402->node()->readObject(_maxTorqueObjectId);
}

void ModeTq::reset()
{
    _nodeProfile402->node()->readObject(_targetObjectId);
}

void ModeTq::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
}
