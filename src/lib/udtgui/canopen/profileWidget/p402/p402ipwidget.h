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

#ifndef P402IPWIDGET_H
#define P402IPWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

class NodeProfile402;
class ModeIp;
class IndexSpinBox;
class IndexLabel;
class IndexCheckBox;

class UDTGUI_EXPORT P402IpWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402IpWidget(QWidget *parent = nullptr);
    ~P402IpWidget() override;

private:
    ModeIp *_modeIp;

    NodeObjectId _positionTargetObjectId;
    NodeObjectId _bufferClearObjectId;
    NodeObjectId _positionDemandValueObjectId;
    NodeObjectId _positionActualValueObjectId;
    NodeObjectId _timePeriodIndexObjectId;
    NodeObjectId _timePeriodUnitsObjectId;

    int _iteratorForSendDataRecord;
    QStringList _listDataRecord;

    QLineEdit *_dataRecordLineEdit;
    QLabel *_infoLabel;

    IndexLabel *_positionDemandValueLabel;
    IndexLabel *_positionAcualValueLabel;

    IndexSpinBox *_timePeriodUnitSpinBox;
    IndexSpinBox *_timePeriodIndexSpinBox;

    QPushButton *_clearBufferPushButton;

    IndexSpinBox *_positionRangeLimitMinSpinBox;
    IndexSpinBox *_positionRangeLimitMaxSpinBox;
    IndexSpinBox *_softwarePositionLimitMinSpinBox;
    IndexSpinBox *_softwarePositionLimitMaxSpinBox;

    IndexSpinBox *_maxProfileVelocitySpinBox;
    IndexSpinBox *_maxMotorSpeedSpinBox;

    IndexSpinBox *_homeOffsetSpinBox;
    IndexCheckBox *_polarityCheckBox;

    QCheckBox *_enableRampCheckBox;

    QSpinBox *_targetPositionSpinBox;
    QCheckBox *_relativeTargetpositionSpinBox;
    QSpinBox *_durationSpinBox;
    QPushButton *_goTargetPushButton;
    QPushButton *_stopTargetPushButton;

    QVector<int> _pointSinusoidalVector;
    QTimer _sendPointSinusoidalTimer;

    void dataRecordLineEditFinished();
    void sendDataRecord();

    void polarityEditingFinished();
    void bufferClearClicked();
    void enableRampClicked(bool ok);
    void enableRampEvent(bool ok);

    void startTargetPosition();
    void stopTargetPosition();
    void calculatePointSinusoidalMotionProfile(qint32 targetPosition, qint32 initialPosition, qreal periodMs);
    void sendDataRecordTargetWithPdo();
    void sendDataRecordTargetWithSdo();

    void updateInformationLabel();

    void dataLogger();
    void pdoMapping();

    // Create widgets
    QFormLayout *_modeLayout;
    void createWidgets();
    void targetWidgets();
    void informationWidgets();
    void limitWidgets();
    void slopeWidgets();
    void homePolarityWidgets();
    QGroupBox *sinusoidalMotionProfileWidgets();
    QGroupBox *controlWordWidgets();
    QHBoxLayout *buttonWidgets();

    // P402Mode interface
public:
    void readRealTimeObjects() override;
    void readAllObjects() override;
    void stop() override;

public slots:
    void setNode(Node *value, uint8_t axis) override;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif  // P402IPWIDGET_H
