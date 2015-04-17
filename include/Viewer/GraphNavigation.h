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

private:

	void setColorConectedNodes( Data::Node* selectedNode );
	void restoreColorConectedNodes( Data::Node* selectedNode );
	void setColorNearestNode( Data::Node* selectedNode );


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
