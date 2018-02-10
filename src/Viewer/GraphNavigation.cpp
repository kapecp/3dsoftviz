#include "Viewer/GraphNavigation.h"
#include "Viewer/PickHandler.h"

#include "QDebug"

#include <leathers/push>
#include <leathers/used-but-marked-unused>

Vwr::GraphNavigation::GraphNavigation() :
	isNavEnabled( true ),
	viewer( AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt() ),
	camMath( nullptr ),
	tempSelectedNode( nullptr ),
	tempSelectedEdge( nullptr ),
	previousLastSelectedNode( nullptr ),
	selectionMode( 2 )
{
}

Vwr::GraphNavigation::~GraphNavigation()
{
}



void Vwr::GraphNavigation::setColorConectedNodes( Data::Node* selectedNode )
{
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* nodeEdges = selectedNode->getEdges();
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iter = nodeEdges->begin(); iter != nodeEdges->end(); ++iter ) {
		// get dest node and possition
		Data::Node* dstNode = ( *iter )->getOtherNode( selectedNode );
		// set temp color for node and edge
		dstNode->setColor( osg::Vec4( 0.0f,1.0f,1.0f,0.5f ) );
		( *iter )->setEdgeColor( osg::Vec4( 0.0f,1.0f,1.0f,0.5f ) );
	}
}

void Vwr::GraphNavigation::restoreColorConectedNodes( Data::Node* selectedNode )
{
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* nodeEdges = selectedNode->getEdges();
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iter = nodeEdges->begin(); iter != nodeEdges->end(); ++iter ) {
		// get dest node and possition
		Data::Node* dstNode = ( *iter )->getOtherNode( selectedNode );
		// restore default color for node and edge
		dstNode->setDefaultColor();
		( *iter )->setEdgeDefaultColor();
	}
}

void Vwr::GraphNavigation::setColorNearestNode( Data::Node* selectedNode )
{
	osg::Vec3f mousePosition = getMouseScreenCoordinates( );
	osg::Vec3f selectedPosition = getNodeScreenCoordinates( selectedNode );

	Data::Edge* closestEdge = NULL;
	double minDistance = 0;
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* nodeEdges = selectedNode->getEdges();
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iter = nodeEdges->begin(); iter != nodeEdges->end(); ++iter ) {
		Data::Node* dstNode = ( *iter )->getOtherNode( selectedNode );
		osg::Vec3f nodePosition = getNodeScreenCoordinates( dstNode );
		double distance = 0;
		// distance to node
		if ( selectionMode == 1 ) {
			distance = getDistanceToNode( mousePosition, nodePosition );
		}
		// distance to edge
		else if ( selectionMode == 2 ) {
			distance = getDistanceToEdge( mousePosition, selectedPosition,nodePosition );
		}
		else {
			// TODO: Add extra conditions for "selectionMode"
		}
		// first edge or nearer node
		if ( qFuzzyCompare( minDistance, 0.0 )  || ( minDistance > distance ) ) {
			minDistance = distance;
			closestEdge = ( *iter );
		}
	}

	// get closest node
	Data::Node* tmpNode = closestEdge->getOtherNode( selectedNode );
	if ( !tmpNode->equals( tempSelectedNode ) ) {
		// remove previous temp select
		if ( tempSelectedNode != NULL ) {
			tempSelectedNode->setColor( osg::Vec4( 0.0f,1.0f,1.0f,0.5f ) );
			tempSelectedEdge->setEdgeColor( osg::Vec4( 0.0f,1.0f,1.0f,0.5f ) );
		}

		// create new temp select for node and edge
		tempSelectedNode = tmpNode;
		tempSelectedNode->setColor( osg::Vec4( 1.0f,1.0f,0.0f,0.5f ) );
		tempSelectedEdge = closestEdge;
		tempSelectedEdge->setEdgeColor( osg::Vec4( 1.0f,1.0f,0.0f,0.5f ) );
	}
}

void Vwr::GraphNavigation::clear()
{
	if ( previousLastSelectedNode != NULL ) {
		restoreColorConectedNodes( previousLastSelectedNode );
	}
	previousLastSelectedNode = NULL;
	tempSelectedNode = NULL;
	tempSelectedEdge = NULL;
}

void Vwr::GraphNavigation::navigate( )
{
	// if there is no selected node, restore default colors
	if ( viewer->getPickHandler()->getSelectedNodes()->isEmpty() ) {
		clear();
	}

	else {
		Data::Node* lastSelectedNode = viewer->getPickHandler()->getSelectedNodes()->last();

		// if last selected node changed
		if ( !lastSelectedNode->equals( previousLastSelectedNode ) ) {
			// restore  default colors in case of extern select ( out of this class methods )
			clear();
			// create new visualized node
			setColorConectedNodes( lastSelectedNode );
			previousLastSelectedNode = lastSelectedNode;
		}

		setColorNearestNode( lastSelectedNode );
	}
}

void Vwr::GraphNavigation::selectNearestNode()
{
	if ( tempSelectedNode!= NULL ) {
		if ( !viewer->getPickHandler()->getSelectedNodes()->contains( tempSelectedNode ) ) {
			viewer->getPickHandler()->addPickedNode( tempSelectedNode );
			tempSelectedNode->setSelected( true );
			clear();
		}
	}
}

void Vwr::GraphNavigation::removeLastSelectedNode()
{
	// if there is no selected node, it can not be removed
	if ( !viewer->getPickHandler()->getSelectedNodes()->isEmpty() ) {
		Data::Node* lastSelectedNode = viewer->getPickHandler()->getSelectedNodes()->last();
		lastSelectedNode->setSelected( false );
		viewer->getPickHandler()->getSelectedNodes()->removeLast();
	}
}

osg::Vec3f Vwr::GraphNavigation::getMouseScreenCoordinates( )
{
	// get mouse coordinates in viewer
	float mouseX = static_cast<float>( viewer->cursor().pos().x() ) - static_cast<float>( viewer->window()->pos().x() ) - 10.0f;
	float mouseY = static_cast<float>( viewer->cursor().pos().y() ) - static_cast<float>( viewer->window()->pos().y() ) - 30.0f;

	// get coordinates inside viewer and invert y
	float xN = mouseX - static_cast<float>( viewer->pos().x() );
	float yN = static_cast<float>( viewer->height() ) + static_cast<float>( viewer->pos().y() ) - mouseY ;

	return osg::Vec3f( xN, yN, 1.0f );
}

osg::Vec3f Vwr::GraphNavigation::getNodeScreenCoordinates( Data::Node* node )
{
	// get matrices ... world->view->projection(device)->viewport(screen)
	osg::Matrixd mwpv = ( viewer->getCamera()->getViewMatrix()
						  * viewer->getCamera()->getProjectionMatrix()
						  * viewer->getCamera()->getViewport()->computeWindowMatrix() );
	// get node matrix ... local->world
	osg::MatrixList ml = node->getWorldMatrices();

	// get node bound position
	osg::Vec3f nodePosition = node->getBound().center();

	// convert node local to world
	for ( osg::MatrixList::iterator iter = ml.begin(); iter != ml.end(); ++iter ) {
		nodePosition = nodePosition*( *iter );
	}
	// convert node world to screen
	nodePosition = ( nodePosition * mwpv );

	return nodePosition;
}

double Vwr::GraphNavigation::getDistanceToNode( osg::Vec3f mouse, osg::Vec3f node )
{
	// in case of big space can overflow ... test divide by 1000
	double distX = fabs( static_cast<double>( node[0] - mouse[0] ) );
	double distY = fabs( static_cast<double>( node[1] - mouse[1] ) );

	return distX*distX + distY*distY;
}

double Vwr::GraphNavigation::getDistanceToEdge( osg::Vec3f mouse, osg::Vec3f line_s, osg::Vec3f line_e )
{
	osg::Vec3f pToLs = ( mouse - line_s );
	osg::Vec3f line = ( line_e - line_s );

	float dot = ( pToLs[0] * line[0] + pToLs[1] * line[1] + pToLs[2] * line[2] );
	float t = dot / line.length2();

	// closest point is line start
	if ( t < 0 ) {
		return getDistanceToNode( mouse, line_s );
	}
	// closest point is line end
	else if ( t > 1 ) {
		return getDistanceToNode( mouse, line_e );
	}
	// closest point is on line
	else {
		osg::Vec3f tmp = ( line_s + line.operator *( t ) );
		return getDistanceToNode( mouse, tmp );
	}
}

#include <leathers/pop>
