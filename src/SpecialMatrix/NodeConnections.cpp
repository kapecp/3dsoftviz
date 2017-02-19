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
	for ( size_t i = 0; i<static_cast<unsigned long>( connectedNodes->size() ); ++i ) {
		delete connectedNodes->value( static_cast<long>( i ) );
	}
	delete connectedNodes;
}

void SpecialMatrix::NodeConnections::setNodePositionsArray( int nodesCount )
{
	nodePositionsArray.resize( static_cast<unsigned long>( nodesCount ) );
	for ( int i = 0; i < nodesCount; ++i ) {
		nodePositionsArray[static_cast<unsigned long>( i )].resize( static_cast<unsigned long>( nodesCount ), 0 );
	}
}

