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

#ifndef MOTORCONFIG_H
#define MOTORCONFIG_H

#include "../../udtgui_global.h"

#include <QWidget>

#include "profile/p402/nodeprofile402.h"

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QToolBar>

class Node;
class AbstractIndexWidget;
class IndexSpinBox;
class IndexLabel;
class IndexComboBox;
class IndexCheckBox;

class UDTGUI_EXPORT MotorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MotorWidget(QWidget *parent = nullptr);

    Node *node() const;
    QString title() const;

public slots:
    void setNode(Node *node, uint8_t axis = 0);

protected:
    Node *_node;
    uint8_t _axis;
    NodeProfile402 *_nodeProfile402;

    // Information
    QLabel *_informationLabel;
    QPushButton *_enableButton;

    // Motor Config
    QGroupBox *_motorConfigGroupBox;
    IndexComboBox *_motorTypeComboBox;
    IndexSpinBox *_peakCurrent;
    IndexSpinBox *_polePair;
    IndexSpinBox *_maxVelocity;
    IndexSpinBox *_velocityConstant;
    IndexSpinBox *_currentConstant;
    IndexCheckBox *_break;
    IndexCheckBox *_reversePolarity;

    // Motor Status
    IndexLabel *_hallRawValueLabel;
    IndexLabel *_hallPhaseLabel;
    IndexLabel *_bridgePweredPhaseLabel;
    IndexLabel *_bridgeCommandLabel;
    IndexLabel *_bridgeTemp1Label;
    IndexLabel *_bridgeTemp2Label;

    void statusNodeChanged(Node::Status status);
    void stateChanged();
    void readAllObject();

    // Creation widgets
    void createWidgets();
    QToolBar *toolBarWidgets();
    QGroupBox *informationWidgets();
    QGroupBox *motorConfigWidgets();
    QGroupBox *motorStatusWidgets();

    void goEnableButton();

    QList<AbstractIndexWidget *> _indexWidgets;
};

#endif  // MOTORCONFIG_H
