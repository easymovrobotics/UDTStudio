#ifndef NODESUBINDEX_H
#define NODESUBINDEX_H

#include "canopen_global.h"
#include <QObject>
#include <QVariant>
#include <QTime>

class NodeIndex;

class CANOPEN_EXPORT NodeSubIndex : public QObject
{
    Q_OBJECT
public:
    NodeSubIndex(const quint8 subIndex);
    NodeSubIndex(const NodeSubIndex &other);
    ~NodeSubIndex();

    quint8 subIndex() const;
    void setSubIndex(const quint8 &subIndex);

    const QString &name() const;
    void setName(const QString &name);

    // ============== Access type =================
    enum AccessType
    {
        NOACESS = 0x00,
        READ = 0x01,
        WRITE = 0x02,
        TPDO = 0x04,
        RPDO = 0x08
    };
    AccessType accessType() const;
    void setAccessType(const AccessType &accessType);

    const QVariant &value() const;
    void setValue(const QVariant &value);
    void clearValue();

    // =============== Data type ==================
    enum DataType
    {
        NONE = 0x0,
        BOOLEAN = 0x0001,
        INTEGER8 = 0x0002,
        INTEGER16 = 0x0003,
        INTEGER32 = 0x0004,
        UNSIGNED8 = 0x0005,
        UNSIGNED16 = 0x0006,
        UNSIGNED32 = 0x0007,
        REAL32 = 0x0008,
        VISIBLE_STRING = 0x0009,
        OCTET_STRING = 0x000A,
        UNICODE_STRING = 0x000B,
        TIME_OF_DAY = 0x000C,
        TIME_DIFFERENCE = 0x000D,
        DDOMAIN = 0x000F,
        INTEGER24 = 0x0010,
        REAL64 = 0x0011,
        INTEGER40 = 0x0012,
        INTEGER48 = 0x0013,
        INTEGER56 = 0x0014,
        INTEGER64 = 0x0015,
        UNSIGNED24 = 0x0016,
        UNSIGNED40 = 0x0018,
        UNSIGNED48 = 0x0019,
        UNSIGNED56 = 0x001A,
        UNSIGNED64 = 0x001B
    };

    DataType dataType() const;
    void setDataType(const DataType &dataType);
    static QString dataTypeStr(const DataType &dataType);

    const QVariant &lowLimit() const;
    void setLowLimit(const QVariant &lowLimit);

    const QVariant &highLimit() const;
    void setHighLimit(const QVariant &highLimit);

    uint8_t flagLimit() const;
    void setFlagLimit(const uint8_t &flagLimit);

    int byteLength() const;

    const QTime &lastModification() const;

private:
    friend class NodeIndex;
    NodeIndex *_nodeIndex;

    quint8 _subIndex;
    QString _name;
    AccessType _accessType;

    QVariant _value;
    QVariant _defaultValue;
    DataType _dataType;

    uint8_t _flagLimit;
    QVariant _lowLimit;
    QVariant _highLimit;

    QTime _lastModification;
};

#endif // NODESUBINDEX_H