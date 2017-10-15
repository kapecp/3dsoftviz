#pragma once

#include "LuaGraphVisualizer.h"

#include <QSet>
#include <osg/Material>

#include "City/Module.h"
#include "City/Building.h"
#include "City/Ball.h"

namespace Lua {


class ModuleGraphVisualizer : public Lua::LuaGraphVisualizer
{

public:

	ModuleGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera = 0 );

	/**
		 * @fn public visualize
		 * @brief load lua nodes and create Data::Graph for module graph
		 */
	void visualize();

	/**
		 * @fn public onUpdate
		 * @brief reload params and labels for nodes / edges
		 */
	void onUpdate();

	/**
		 * @fn public onUpdate
		 * @brief reload params and labels for nodes / edges
		 */

private:

	/**
		 * @fn private deleteReturnsNode
		 * @brief delete helper node Returns from graph
		 */
	void deleteReturnsNode();

	/**
		 * @fn private adjustBuildingForNode
		 * @brief adjust building for node according to passed arguments
		 * @param osg::ref_ptr	pointer to Data::Node
		 * @param float	baseSize
		 * @param float	height
		 * @param bool	lieOnGround
		 * @param osg::ref_ptr	colour
		 * @param QString	textureName
		 */
	void adjustBuildingForNode( osg::ref_ptr<Data::Node> node, float baseSize, float height, bool lieOnGround, osg::ref_ptr<osg::Material> colour, QString textureName );

	/**
		 * @fn private adjustBallForNode
		 * @brief adjust ball for node according to passed arguments
		 * @param osg::ref_ptr	pointer to Data::Node
		 * @param float	baseSize
		 * @param bool	lieOnGround
		 * @param osg::ref_ptr	colour
		 */
	void adjustBallForNode( osg::ref_ptr<Data::Node> node, float baseSize, bool lieOnGround, osg::ref_ptr<osg::Material> colour );

	/**
		 * QStringList hierarchyEdges
		 * @brief list of Strings containing names of hierarchy edges
		 */
	QStringList hierarchyEdges = QStringList() << "provides" << "initializes" << "declares";

	/**
		 * QStringList arcEdges
		 * @brief list of Strings containing names of arc edges
		 */
	QStringList arcEdges = QStringList() << "calls" << "requires" << "represents" << "contains";
};

}


