
#include "Data/Cluster.h"

#include <QLinkedList>

namespace Data {

Cluster::Cluster( qlonglong id, QString name, Data::Type* type, float scaling, Data::Graph* graph, osg::Vec3f position ) :
	Node( id, name, type, scaling, graph, position ),

	id( id ),
	repulsiveForceInside( 1 ),
	cube( nullptr ),
	sphere( nullptr ),
	shapeGetter( nullptr )
{
}

QSet<Node*> Cluster::getIncidentNodes() const
{
	QSet<Node*> nodes;

	QSet<Node*>::const_iterator i = clusteredNodes.constBegin();
	while ( i != clusteredNodes.constEnd() ) {
		Node* v = *i;
		nodes.unite( v->getIncidentNodes() );
		++i;
	}
	nodes.subtract( getALLClusteredNodes() );

	Cluster* n = const_cast<Cluster*>( this );
	nodes.remove( n );
	nodes.remove( n );

	return nodes;
}

QSet<Node*> Cluster::getALLClusteredNodes() const
{
	QSet<Node*> nodes;

	QSet<Node*>::const_iterator i = clusteredNodes.constBegin();
	while ( i != clusteredNodes.constEnd() ) {
		Node* clusteredNode = *i;

		Cluster* cluster = dynamic_cast<Cluster*>( clusteredNode );
		if ( cluster != 0 ) { // je to cluster
			nodes.unite( cluster->getALLClusteredNodes() );
		}
		else {   // je to node
			nodes.insert( clusteredNode );
		}
		++i;
	}

	return nodes;
}

osg::Vec3 Cluster::getMidpoint()
{
	osg::Vec3 total = osg::Vec3( 0,0,0 );
	int count = 0;

	QSet<Data::Node*>::const_iterator i = clusteredNodes.constBegin();
	while ( i != clusteredNodes.constEnd() ) {

		Data::Node* v = *i;

		osg::Vec3f pos = v->getCurrentPosition();

		total += pos;

		++i;
		++count;
	}
	total.operator /=( static_cast<float>( count ) );

	return total;
}

void Cluster::setSelected( bool selected, QLinkedList<osg::ref_ptr<Data::Node> >* pickedNodes )
{
	this->selected = selected;
	if ( selected ) {
		originalColor = getColor();
		setColor( osg::Vec4( 0,1,0,1 ) );

		foreach ( Node* node, clusteredNodes ) {
			Cluster* c = dynamic_cast<Cluster*>( node );
			if ( c != 0 ) {
				c->setSelected( selected, pickedNodes );
			}
			else {
				node->setSelected( selected );
				pickedNodes->append( node );
			}
		}

	}
	else {
		setColor( originalColor );

		foreach ( Node* node, clusteredNodes ) {
			Cluster* c = dynamic_cast<Cluster*>( node );
			if ( c != 0 ) {
				c->setSelected( selected, pickedNodes );
			}
			else {
				node->setSelected( selected );
				pickedNodes->removeOne( node );
			}
		}
	}
}

Node* Cluster::getRandomNode()
{
	foreach ( Node* node, clusteredNodes ) {
		Cluster* c = dynamic_cast<Cluster*>( node );
		if ( c != 0 ) {
			return c->getRandomNode();
		}
		else {
			return node;
		}
	}
	return NULL;
}

void Cluster::setRepulsiveForceInside( double repulsiveForceInside )
{
	this->repulsiveForceInside = repulsiveForceInside;

	QSet<Node*>::const_iterator i = clusteredNodes.constBegin();
	while ( i != clusteredNodes.constEnd() ) {
		Node* clusteredNode = *i;

		Cluster* cluster = dynamic_cast<Cluster*>( clusteredNode );
		if ( cluster != 0 ) { // je to cluster
			cluster->setRepulsiveForceInside( repulsiveForceInside );
		}
		++i;
	}
}

} // namespace Data
