#include "Data/VizEdge.h"
#include <QTextStream>

Data::VizEdge::VizEdge( qlonglong id, QString name, Data::Graph* graph, bool isOriented, Data::Type* type, float scaling )
	:AbsEdge( id,name, graph, isOriented, type )
{
	this->isInvisible = false;
	this->scale = scaling;
	this->setSharedCoordinates( false, false, false );

}

QString Data::VizEdge::toString() const
{
	QString str;
	QTextStream( &str ) << "edge id:" << id << " name:" << name;
	return str;
}
