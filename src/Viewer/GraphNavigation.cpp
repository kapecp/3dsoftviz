#include "Viewer/GraphNavigation.h"
#include "QDebug"

Vwr::GraphNavigation::GraphNavigation()
{
	isNavEnabled = true;
	tempSelectedNode = NULL;
	tempSelectedEdge = NULL;
	previousLastSelectedNode = NULL;
	viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
}

Vwr::GraphNavigation::~GraphNavigation()
{
}



void Vwr::GraphNavigation::setColorConectedNodes( Data::Node* selectedNode )
{
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* nodeEdges = selectedNode->getEdges();
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iter = nodeEdges->begin(); iter != nodeEdges->end(); iter++ ) {
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
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iter = nodeEdges->begin(); iter != nodeEdges->end(); iter++ ) {
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

	Data::Edge* closestEdge = NULL;
	float minDistance = 0;
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* nodeEdges = selectedNode->getEdges();
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iter = nodeEdges->begin(); iter != nodeEdges->end(); iter++ ) {
		Data::Node* dstNode = ( *iter )->getOtherNode( selectedNode );
		osg::Vec3f nodePosition = getNodeScreenCoordinates( dstNode );
		double distance = getDistance( mousePosition, nodePosition );

		// first edge or nearer node
		if ( ( minDistance == 0 ) || ( minDistance > distance ) ) {
			minDistance = distance;
			closestEdge = ( *iter );
		}
	}

	//printf( "[ %.3f - %.3f ]/[ %.3f - %.3f ]\n",mousePossition[0],mousePossition[1],nodePossition[0],nodePossition[1] );

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

void Vwr::GraphNavigation::navigate()
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
		viewer->getPickHandler()->addPickedNode( tempSelectedNode );
		tempSelectedNode->setSelected( true );
		clear();
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
	float mouseX = viewer->cursor().pos().x() - viewer->window()->pos().x() - 10;
	float mouseY = viewer->cursor().pos().y() - viewer->window()->pos().y() - 30;

	// get coordinates inside viewer and invert y
	float xN = static_cast<float>( mouseX - viewer->pos().x() );
	float yN = static_cast<float>( viewer->height() + viewer->pos().y() - mouseY );

	return osg::Vec3f( xN, yN, 0.0f );
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

double Vwr::GraphNavigation::getDistance( osg::Vec3f mouse, osg::Vec3f node )
{
	// in case of big space can overflow ... test divide by 1000
	double distX = abs( node[0] - mouse[0] )/1000;
	double distY = abs( node[1] - mouse[1] )/1000;

	return sqrt( distX*distX + distY*distY );
}
