#include "Data/AbsEdge.h"

#include <QString>

Data::AbsEdge::AbsEdge( qlonglong id, QString name, Graph* graph, bool isOriented, Data::Type* type )
{
	this->id=id;
	this->name=name;
	this->type = type;
	this->graph = graph;
	this->oriented = isOriented;
	this->edgeStrength = 1;
	this->selected = false;
}

