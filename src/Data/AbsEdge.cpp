#include "Data/AbsEdge.h"
#include <QString>

Data::AbsEdge::AbsEdge( qlonglong id, QString name)
{
    this->id=id;
    this->name=name;
}

