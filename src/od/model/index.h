#ifndef INDEX_H
#define INDEX_H

#include <QList>
#include <QString>

#include <stdint.h>
#include "subindex.h"
#include "datatype.h"

class Index : public SubIndex
{
public:

    Index();
    Index(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName, const uint16_t &index, const uint8_t &nbSubIndex);
    ~Index();

    uint16_t index() const;
    void setIndex(const uint16_t &index);

    uint8_t nbSubIndex() const;
    void setNbSubIndex(const uint8_t &nbSubIndex);

    QList<SubIndex *> &subIndexes();
    SubIndex* subIndex(const uint8_t &subIndexKey) const;
    void addSubIndex(SubIndex *subIndex);

private:

    uint16_t            _index;
    uint8_t             _nbSubIndex;
    QList<SubIndex*>    _subIndexes;
};

#endif // INDEX_H
