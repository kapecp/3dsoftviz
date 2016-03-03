#include "Data/GraphSerializer.h"

#include <vector>

namespace Data {

//Nepouziva sa
osg::Vec3f GraphSerializer::getRandomPosition()
{
	//nastavime nahodne inicializnacne suradnice v 3D
	double x, y, z;
	double lowest = 1.0;
	double highest = 100.0;
	double range = ( highest - lowest ) + 1.0;

	srand( static_cast<unsigned>( time( 0 ) ) );

	x = lowest + ( range * static_cast<double>( rand() ) / ( RAND_MAX + 1.0 ) );
	y = lowest + ( range * static_cast<double>( rand() ) / ( RAND_MAX + 1.0 ) );
	z = lowest + ( range * static_cast<double>( rand() ) / ( RAND_MAX + 1.0 ) );

	return osg::Vec3f( static_cast<float>( x ), static_cast<float>( y ), static_cast<float>( z ) );
}

GraphSerializer::GraphSerializer( std::vector<Data::Node*>* in_nodes,std::vector<Data::Edge*>* in_edges,std::vector<Data::Type*>* in_types )
{
	nodes = in_nodes;
	edges = in_edges;
	types = in_types;

	QString tNStr( "node" );
	QString tEStr( "edge" );

	Data::Type* nType = new Data::Type( 0,tNStr,0 );
	Data::Type* eType = new( std::nothrow ) Data::Type( 1,tEStr,0 );

	types->insert( types->end(),nType );
	types->insert( types->end(),eType );

	nodeIdSeq = 0;
	edgeIdSeq = 0;
}

bool GraphSerializer::edgeExists( Data::Edge* e )
{
	for ( unsigned int x = 0; x < edges->size(); x++ ) {
		if ( edges->at( x )->getSrcNode()->getId() == e->getSrcNode()->getId()
				&& edges->at( x )->getDstNode()->getId() == e->getDstNode()->getId() ) {
			return true;
		}
	}

	return false;
}

void GraphSerializer::addEntry( QString edgeId, QString edgeData, QString incId, QString incData, QString nodeId, QString nodeData )
{
	if ( !nodeIdMap.contains( edgeId ) ) {
		nodes->push_back( createNode( edgeId, edgeData ) );
	}

	if ( !nodeIdMap.contains( incId ) ) {
		nodes->push_back( createNode( incId, incData ) );
	}

	if ( !nodeIdMap.contains( nodeId ) ) {
		nodes->push_back( createNode( nodeId, nodeData ) );
	}

	Data::Edge* edge1 = new Data::Edge( edgeIdSeq, "edge data", NULL, nodes->at( static_cast<std::size_t >( nodeIdMap.value( edgeId ) ) ), nodes->at( static_cast<std::size_t >( nodeIdMap.value( incId ) ) ), types->at( 1 ), true, 2 );
	if ( !edgeExists( edge1 ) ) {
		edgeIdSeq++;
	}

	Data::Edge* edge2 = new Data::Edge( edgeIdSeq, "edge data", NULL, nodes->at( static_cast<std::size_t >( nodeIdMap.value( incId ) ) ), nodes->at( static_cast<std::size_t >( nodeIdMap.value( nodeId ) ) ), types->at( 1 ), true, 2 );
	if ( !edgeExists( edge2 ) ) {
		edgeIdSeq++;
	}
}

Data::Node* GraphSerializer::createNode( QString id, QString data )
{
	nodeIdMap.insert( id, nodeIdSeq++ );

	return new Data::Node( nodeIdMap.value( id ), data, types->at( 0 ), 0, NULL, getRandomPosition() );
}

} // namespace Data
