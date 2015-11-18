#include "Data/GraphSerializer.h"

#include <vector>

namespace Data {

//Nepouziva sa
osg::Vec3f GraphSerializer::getRandomPosition()
{
	//nastavime nahodne inicializnacne suradnice v 3D
	float x, y, z;
	float lowest = 1.f;
	float highest = 100.f;
	float range = ( highest - lowest ) + 1.f;

	srand( ( unsigned )time( 0 ) );

	x = lowest + static_cast<float>( range * static_cast<float>( rand() ) / ( static_cast<float>( RAND_MAX ) + 1.f ) );
	y = lowest + static_cast<float>( range * static_cast<float>( rand() ) / ( static_cast<float>( RAND_MAX ) + 1.f ) );
	z = lowest + static_cast<float>( range * static_cast<float>( rand() ) / ( static_cast<float>( RAND_MAX ) + 1.f ) );

	return osg::Vec3f( x, y, z );
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
