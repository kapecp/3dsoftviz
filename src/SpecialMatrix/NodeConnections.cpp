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

SpecialMatrix::NodeConnections::~NodeConnections()
{
	delete xAxisNodes;
	delete yAxisNodes;
	delete iNodes;
	for ( size_t i = 0; i<connectedNodes->size(); ++i ) {
		delete connectedNodes->value( i );
	}
	delete connectedNodes;
}

void SpecialMatrix::NodeConnections::setNodePositionsArray( int nodesCount )
{
	nodePositionsArray.resize( nodesCount );
	for ( int i = 0; i < nodesCount; ++i ) {
		nodePositionsArray[i].resize( nodesCount, 0 );
	}
}

