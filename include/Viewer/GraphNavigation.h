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
	void navigate();
	void selectNearestNode();
	void removeLastSelectedNode();
	void setSelectionMode( int mode )
	{
		this->selectionMode = mode;
	}
	bool isNavEnabled;

private:

	void setColorConectedNodes( Data::Node* selectedNode );
	void restoreColorConectedNodes( Data::Node* selectedNode );
	// selectionMode 1=node 2=edge
	void setColorNearestNode( Data::Node* selectedNode );
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

	int selectionMode;
};
}

#endif
