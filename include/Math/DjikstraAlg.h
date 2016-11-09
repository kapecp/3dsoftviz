#pragma once

#include "Data/Node.h"
#include "Data/Edge.h"

#include <QLinkedList>
#include <QVector>
#include <QSet>

namespace Data {
class Graph;
}
namespace MathModule {

class DjikstraAlg
{
public:
	explicit DjikstraAlg( Data::Graph* graph );
	~DjikstraAlg( void );

	QMap<qlonglong, qlonglong > execute( Data::Node* node );
	QVector<QLinkedList<osg::ref_ptr<Data::Node> > > getAllPaths( Data::Node* target );

private:
	const QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes;
	const QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges;

	void getPath( Data::Node* target, QLinkedList<osg::ref_ptr<Data::Node> >* path = 0 );
	QVector<QLinkedList<osg::ref_ptr<Data::Node> > > allPaths;

	QSet<qlonglong> settledNodes;
	QSet<qlonglong> unSettledNodes;

	QMap<qlonglong, qlonglong > predecessors;
	QMap<qlonglong, qlonglong > distances;

	void findMinimalDistances( Data::Node* node );
	QVector<osg::ref_ptr<Data::Node> > getNeighbors( Data::Node* node );
	Data::Node* getMinimum( QSet<qlonglong> vertexes );
	int getDistance( Data::Node* node, Data::Node* target );
	bool isSettled( Data::Node* vertex );

	qlonglong getShortestDistance( Data::Node* destination );
};
}
