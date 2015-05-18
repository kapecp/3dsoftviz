#include "Data/DbNode.h"

Data::DbNode::DbNode( qlonglong id, QString name, Data::Type* type, Data::Graph* graph, float scaling )
	:VizNode( id,name,type,graph,scaling )
{
	this->inDB = false;
}
