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

#ifndef HEXMERGER_H
#define HEXMERGER_H

#include "canopen_global.h"

#include <QByteArray>
#include <QStringList>

class CANOPEN_EXPORT HexMerger
{
public:
    HexMerger();

    int merge(const QString fileA, QStringList segmentA, const QString fileB, QStringList segmentB);
    int merge(const QByteArray &appA, QStringList segmentA, const QByteArray &appB, QStringList segmentB);

    const QByteArray &prog() const;

    void setValidProgram(QString adress, QString type);

private:
    QByteArray _prog;

    int append(const QByteArray &app, QStringList addresses);
    int checkAddresses(QStringList addresses);
};

#endif // HEXMERGER_H
