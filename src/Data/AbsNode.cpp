#include "Data/AbsNode.h"
#include <QString>

Data::AbsNode::AbsNode( qlonglong id, QString name)
{
    this->id=id;
    this->name=name;
}
