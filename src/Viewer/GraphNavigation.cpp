#include "Viewer/GraphNavigation.h"
#include "QDebug"

Vwr::GraphNavigation::GraphNavigation()
{
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
	float mouseX = viewer->cursor().pos().x();
	float mouseY = viewer->cursor().pos().y();

	Data::Edge* closestEdge = NULL;
	float minDistance = 0;
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* nodeEdges = selectedNode->getEdges();
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iter = nodeEdges->begin(); iter != nodeEdges->end(); iter++ ) {
		// get dest node and possition
		Data::Node* dstNode = ( *iter )->getOtherNode( selectedNode );
		// convert node coordinates to screen coordinates
		osg::Vec3f nodePossition = camMath->projectOnScreen( viewer->getCamera(), dstNode->getCurrentPosition() );
		// calculate distance between node and cursor
		double distance = sqrt( pow( abs( nodePossition[0] - mouseX ), 2 )
								+ pow( abs( nodePossition[1] - mouseY ), 2 ) );

		// first edge or nearer node
		if ( ( minDistance == 0 ) || ( minDistance > distance ) ) {
			minDistance = distance;
			closestEdge = ( *iter );
		}
	}
	osg::Vec3f nodePossition = camMath->projectOnScreen( viewer->getCamera(), selectedNode->getCurrentPosition() );
	printf( "[ %.3f - %.3f ]/[ %.3f - %.3f ]\n",mouseX,mouseY,nodePossition[0],nodePossition[1] );
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
			if ( previousLastSelectedNode != NULL ) {
				restoreColorConectedNodes( previousLastSelectedNode );
			}
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
