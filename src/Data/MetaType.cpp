/*!
 * MetaType.cpp
 * Projekt 3DVisual
 */

#include "Data/MetaType.h"

#include "Data/Graph.h"
#include "Data/GraphLayout.h"

Data::MetaType::MetaType(qlonglong id, QString name, Data::Graph* graph, Data::GraphLayout* layout, QMap<QString, QString> * settings) : Data::Type(id,name,graph,settings)
{
	this->layout = layout; this->meta = true;
}



Data::MetaType::~MetaType(void)
{
}
