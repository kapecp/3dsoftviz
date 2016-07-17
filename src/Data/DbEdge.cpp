#include "Data/DbEdge.h"

Data::DbEdge::DbEdge( qlonglong id, QString name, Data::Graph* graph, bool isOriented, Data::Type* type, double scaling )
	:VizEdge( id,name, graph, isOriented, type, scaling )
{
	this->inDB = false;
}
