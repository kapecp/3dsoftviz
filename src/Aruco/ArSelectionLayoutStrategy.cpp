#include "Aruco/ArSelectionLayoutStrategy.h"
#include <QDebug>
#include <QMap>

#include "Data/Node.h"


namespace ArucoModul {





/*
 * ArSelectionLayoutStrategy
 */
ArSelectionLayoutStrategy::ArSelectionLayoutStrategy()
{

}

void ArSelectionLayoutStrategy::setSelectionLayout( osg::ref_ptr<Data::Node> node )
{
	setSelectedNodeLayoutOptions( node );
}
void ArSelectionLayoutStrategy::resetSelectionLayout( osg::ref_ptr<Data::Node> node, bool isReturnNode )
{
	resetSelectedNodeLayoutOptions( node, isReturnNode );
}





/*
 * ArSelectionLayoutStrategyNodeOnly
 */
void ArSelectionLayoutStrategyNodeOnly::setSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node )
{
	this->original_position = node->getTargetPosition();

	node->setDrawableColor( osg::Vec4( 0.0f,1.0f,0.0f,1.0f ) );
	node->setUsingInterpolation( false );
	node->setIgnoreByLayout( true );

	for ( auto j : node->getEdges()->keys() ) {
		node->getEdges()->value( j )->getOtherNode( node )->setIgnoreByLayout( true );
		node->getEdges()->value( j )->getOtherNode( node )->setDrawableColor( osg::Vec4( 0.0f,1.0f,0.0f,0.2f ) );
	}
}
void ArSelectionLayoutStrategyNodeOnly::resetSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node, bool isReturnNode )
{
	if ( isReturnNode ) {
		node->setTargetPosition( this->original_position );
	}

	node->setDefaultColor();
	node->setUsingInterpolation( true );
	node->setIgnoreByLayout( false );

	for ( auto j : node->getEdges()->keys() ) {
		node->getEdges()->value( j )->getOtherNode( node )->setIgnoreByLayout( false );
		node->getEdges()->value( j )->getOtherNode( node )->setDefaultColor();
	}
}





/*
 * ArSelectionLayoutStrategyNodeCluster
 */
void ArSelectionLayoutStrategyNodeCluster::setSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node )
{
	node->setDrawableColor( osg::Vec4( 0.0f,1.0f,0.0f,1.0f ) );
	node->setUsingInterpolation( false );
	node->setIgnoreByLayout( true );
}
void ArSelectionLayoutStrategyNodeCluster::resetSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node, bool isReturnNode )
{
	node->setDefaultColor();
	node->setUsingInterpolation( true );
	node->setIgnoreByLayout( false );
}





}
