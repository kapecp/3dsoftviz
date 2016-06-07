#include "Util/ElementSelector.h"

#include "Viewer/PickHandler.h"
#include "Data/Node.h"
#include "Data/Edge.h"

void Util::ElementSelector::randomElementSelector( QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, int num, Vwr::PickHandler* pickhandler )
{
	pickhandler->unselectPickedEdges();
	pickhandler->unselectPickedNodes();

	int numNodes = qrand() % ( num + 1 );
	int numEdges = num - numNodes;

	for ( int x = 0; x < numNodes; x++ ) {
		osg::ref_ptr<Data::Node> node = nodes->value( nodes->uniqueKeys().at( qrand() % nodes->size() ) );

		node->setSelected( true );
		pickhandler->addPickedNode( node );
	}

	for ( int x = 0; x < numEdges; x++ ) {
		osg::ref_ptr<Data::Edge> edge = edges->value( edges->uniqueKeys().at( qrand() % edges->size() ) );

		edge->setSelected( true );
		pickhandler->addPickedEdge( edge );
	}
}

void Util::ElementSelector::weightedElementSelector( QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, int num, Vwr::PickHandler* pickhandler )
{
	pickhandler->unselectPickedEdges();
	pickhandler->unselectPickedNodes();

	QMap<float, osg::ref_ptr<Data::Node> > weightedNodes;

	QMutableMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i( *nodes );

	while ( i.hasNext() ) {
		i.next();

		weightedNodes.insertMulti( i.value()->getOverallWeight(), i.value() );
	}

	QMutableMapIterator<float, osg::ref_ptr<Data::Node> > i2( weightedNodes );
	int cnt = 0;
	i2.toBack();

	while ( i2.hasPrevious() && cnt < num ) {
		i2.previous();

		i2.value()->setSelected( true );
		pickhandler->addPickedNode( i2.value() );
		cnt++;
	}
}
