#include "SpecialMatrix/NodeConnections.h"

#include "osg/LineWidth"
#include "Importer/GraphOperations.h"

#include <QDebug>

SpecialMatrix::NodeConnections::NodeConnections()
{
	xAxisNodes = new QList<qlonglong>;
	yAxisNodes = new QList<qlonglong>;
	iNodes = new QList<qlonglong>;
	connectedNodes = new QMap<qlonglong, QList<qlonglong>* >;
}


