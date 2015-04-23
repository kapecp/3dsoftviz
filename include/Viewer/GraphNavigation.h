#ifndef GRAPHNAVIGATION_H
#define GRAPHNAVIGATION_H

#include "Core/Core.h"
#include "Math/CameraMath.h"


namespace Vwr {
/**
*@brief Navigation in graph
*@author Juraj Marak
*/
class GraphNavigation
{
public:
	/**
	 * @brief GraphNavigation
	 */
	GraphNavigation();
	~GraphNavigation();

	void clear();
	void navigate( int selectionMode );
	void selectNearestNode();
	void removeLastSelectedNode();

	bool isNavEnabled;

private:

	void setColorConectedNodes( Data::Node* selectedNode );
	void restoreColorConectedNodes( Data::Node* selectedNode );
	// selectionMode 1=node 2=edge
	void setColorNearestNode( Data::Node* selectedNode , int selectionMode );
	osg::Vec3f getMouseScreenCoordinates( );
	osg::Vec3f getNodeScreenCoordinates( Data::Node* node );
	double getDistanceToNode( osg::Vec3f mouse, osg::Vec3f node );
	double getDistanceToEdge( osg::Vec3f mouse, osg::Vec3f line_s, osg::Vec3f line_e );

	/**
	 * @brief viewer
	 */
	QOSG::ViewerQT* viewer;

	/**
	 * @brief math module
	 */
	MathModule::CameraMath* camMath;


	Data::Node* tempSelectedNode;
	Data::Edge* tempSelectedEdge;
	Data::Node* previousLastSelectedNode;
};
}

#endif
