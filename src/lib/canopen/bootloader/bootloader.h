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

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"

#include <QObject>

class Node;
class NodeObjectId;
class UfwParser;
class UfwUpdate;

class CANOPEN_EXPORT Bootloader : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    Bootloader(Node *node);

    quint8 busId() const;
    quint8 nodeId() const;
    Node *node() const;

    enum StatusProgram
    {
        NONE,
        PROGRAM_STOPPED,
        PROGRAM_STARTED,
        NO_PROGRAM,
        UPDATE_IN_PROGRESS,
        SUCCESSFUL,
        NOT_SUCCESSFUL
    };
    StatusProgram statusProgram();

    bool openUfw(const QString &fileName);

    void update();

public slots:
    void stopProgram();
    void startProgram();
    void resetProgram();
    void clearProgram();
    void sendKey(uint16_t key);

signals:
    void isUpdated(bool ok);

private slots:
    void getStatusProgram();
    void isUploaded(bool ok);

private:
    Node *_node;

    StatusProgram _statusProgram;

    NodeObjectId _bootloaderKeyObjectId;
    NodeObjectId _bootloaderChecksumObjectId;
    NodeObjectId _bootloaderStatusObjectId;
    NodeObjectId _programObjectId;
    NodeObjectId _programControlObjectId;

    UfwParser *_ufw;
    UfwUpdate *_ufwUpdate;

    QTimer *_timer;

    enum BootloaderState
    {
        STATE_FREE,
        STATE_CHECK_MODE,
        STATE_STOP_PROGRAM,
        STATE_CLEAR_PROGRAM,
        STATE_UPDATE_PROGRAM,
        STATE_UPLOADED_FINISHED,
        STATE_CHECK,
        STATE_OK,
        STATE_NOT_OK
    };

    BootloaderState _state;
    QTimer _statusTimer;

    void checkSoftware();
    void process();
    void uploadProgram();
    void uploadFinishedProgram();
    uint8_t calculateCheckSum(const QByteArray &prog);

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // BOOTLOADER_H
