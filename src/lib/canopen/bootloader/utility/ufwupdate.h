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

#ifndef UFWUPDATE_H
#define UFWUPDATE_H

#include "canopen_global.h"

#include "../parser/ufwparser.h"
#include "nodeodsubscriber.h"

#include <QObject>

class Node;
class NodeObjectId;

class CANOPEN_EXPORT UfwUpdate : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    UfwUpdate(Node *node, UfwParser *ufw = nullptr);

    void setUfw(UfwParser *ufw);

    int update();

    int status();

    uint8_t checksum() const;

signals:
    void isUploaded(bool);

private:
    Node *_node;
    UfwParser *_ufw;
    uint8_t _checksum;
    NodeObjectId _programDataObjectId;

    int _indexList;
    QList<QByteArray> _byteArrayList;
    void process();
    void calculateCheckSum(const QByteArray &prog);

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // UFWUPDATE_H
