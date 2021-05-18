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

#ifndef NODESCREENOD_H
#define NODESCREENOD_H

#include "udtgui_global.h"

#include "nodescreen.h"

#include <canopen/nodeod/nodeodwidget.h>

class UDTGUI_EXPORT NodeScreenOD : public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenOD();

protected:
    void createWidgets();
    NodeOdWidget *_nodeOdWidget;

    QWidget *createStoreWidget();
    QComboBox *_storeComboBox;
    QComboBox *_restoreComboBox;

private slots:
    void storeClicked();
    void restoreClicked();

    // NodeScreen interface
public:
    QString title() const override;
    void setNodeInternal(Node *node, uint8_t axis = 0) override;
};

#endif // NODESCREENOD_H
