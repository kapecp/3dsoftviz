#include "Data/VizEdge.h"
#include <QTextStream>

Data::VizEdge::VizEdge(qlonglong id, QString name, Data::Graph* graph, bool isOriented, Data::Type* type, float scaling)
:AbsEdge(id,name, graph, isOriented, type)
{
    this->scale = scaling;
    this->setSharedCoordinates( false, false, false );
    float r = type->getSettings()->value( "color.R" ).toFloat();
    float g = type->getSettings()->value( "color.G" ).toFloat();
    float b = type->getSettings()->value( "color.B" ).toFloat();
    float a = type->getSettings()->value( "color.A" ).toFloat();

    this->edgeColor = cv::Vec4f( r, g, b, a );
}

QString Data::VizEdge::toString() const
{
    QString str;
    QTextStream( &str ) << "edge id:" << id << " name:" << name;
    return str;
}
